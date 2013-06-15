/*
This file is part of WME Lite.
http://dead-code.org/redir.php?target=wmelite

Copyright (c) 2011 Jan Nedoma

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "dcgf.h"
#include "BFileManager.h"
#include "StringUtil.h"
#include "PathUtil.h"
#include "FileOperations.h"

#ifdef __WIN32__
#	include <direct.h>
#else
#	include <unistd.h>
#endif

#ifdef __APPLE__
#	include <CoreFoundation/CoreFoundation.h>
#endif

#ifdef __ANDROID__
#	include "android/android.h"
#endif

#if _DEBUG
	#pragma comment(lib, "zlib_d.lib")
#else
	#pragma comment(lib, "zlib.lib")
#endif


extern "C"
{
	#include "zlib.h"
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
CBFileManager::CBFileManager(CBGame* inGame):CBBase(inGame)
{
	m_BasePath = NULL;

	InitPaths();
	RegisterPackages();
}


//////////////////////////////////////////////////////////////////////
CBFileManager::~CBFileManager()
{
	Cleanup();
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::Cleanup()
{
	int i;

	// delete registered paths
	for(i=0; i<m_SinglePaths.GetSize(); i++)
		delete [] m_SinglePaths[i];
	m_SinglePaths.RemoveAll();

	for(i=0; i<m_PackagePaths.GetSize(); i++)
		delete [] m_PackagePaths[i];
	m_PackagePaths.RemoveAll();


	// delete file entries
	m_FilesIter = m_Files.begin();
	while(m_FilesIter!=m_Files.end())
	{
		delete m_FilesIter->second;
		m_FilesIter++;
	}
	m_Files.clear();

	// close open files
	for(i=0; i<m_OpenFiles.GetSize(); i++)
	{
		m_OpenFiles[i]->Close();
		delete m_OpenFiles[i];
	}
	m_OpenFiles.RemoveAll();


	// delete packages
	for(i=0; i<m_Packages.GetSize(); i++)
		delete m_Packages[i];
	m_Packages.RemoveAll();

	SAFE_DELETE_ARRAY(m_BasePath);

	return S_OK;
}



#define MAX_FILE_SIZE 10000000
//////////////////////////////////////////////////////////////////////
BYTE* CBFileManager::ReadWholeFile(const char* Filename, DWORD* Size, bool MustExist)
{

	BYTE* buffer=NULL;
	
	CBFile* File = OpenFile(Filename);
	if(!File)
	{
		if(MustExist) Game->LOG(0, "Error opening file '%s'", Filename);
		return NULL;
	}

	/*
	if(File->GetSize()>MAX_FILE_SIZE){
		Game->LOG(0, "File '%s' exceeds the maximum size limit (%d bytes)", Filename, MAX_FILE_SIZE);
		CloseFile(File);
		return NULL;
	}
	*/


	buffer = new BYTE[File->GetSize()+1];
	if(buffer == NULL)
	{
		Game->LOG(0, "Error allocating buffer for file '%s' (%d bytes)", Filename, File->GetSize()+1);
		CloseFile(File);
		return NULL;
	}

	if(FAILED(File->Read(buffer, File->GetSize())))
	{
		Game->LOG(0, "Error reading file '%s'", Filename);
		CloseFile(File);
		delete [] buffer;
		return NULL;
	};

	buffer[File->GetSize()] = '\0';
	if(Size!=NULL) *Size = File->GetSize();
	CloseFile(File);

	return buffer;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::SaveFile(char *Filename, BYTE *Buffer, DWORD BufferSize, bool Compressed, BYTE* PrefixBuffer, DWORD PrefixSize)
{
	RestoreCurrentDir();

	CBUtils::CreatePath(Filename, false);
	
	FILE* f = fopen(Filename, "wb");
	if(!f)
	{
		Game->LOG(0, "Error opening file '%s' for writing.", Filename);
		return E_FAIL;
	}

	if(PrefixBuffer && PrefixSize)
	{
		fwrite(PrefixBuffer, PrefixSize, 1, f);
	}

	if(Compressed)
	{
		DWORD CompSize = BufferSize + (BufferSize / 100) + 12; // 1% extra space
		BYTE* CompBuffer = new BYTE[CompSize];
		if(!CompBuffer)
		{
			Game->LOG(0, "Error allocating compression buffer while saving '%s'", Filename);
			Compressed = false;
		}
		else
		{
			if(compress(CompBuffer, (uLongf*)&CompSize, Buffer, BufferSize)==Z_OK)
			{
				DWORD magic = DCGF_MAGIC;
				fwrite(&magic, sizeof(DWORD), 1, f);
				magic = COMPRESSED_FILE_MAGIC;
				fwrite(&magic, sizeof(DWORD), 1, f);

				DWORD DataOffset = 5*sizeof(DWORD);
				fwrite(&DataOffset, sizeof(DWORD), 1, f);

				fwrite(&CompSize, sizeof(DWORD), 1, f);
				fwrite(&BufferSize, sizeof(DWORD), 1, f);

				fwrite(CompBuffer, CompSize, 1, f);
			}
			else
			{
				Game->LOG(0, "Error compressing data while saving '%s'", Filename);
				Compressed = false;
			}

			delete [] CompBuffer;
		}
	}
	
	if(!Compressed) fwrite(Buffer, BufferSize, 1, f);

	fclose(f);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::RequestCD(int CD, char *PackageFile, char *Filename)
{
	// unmount all non-local packages
	for(int i=0; i<m_Packages.GetSize(); i++)
	{
		if(m_Packages[i]->m_CD > 0) m_Packages[i]->Close();
	}


	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::AddPath(TPathType Type, const char *Path)
{
	if(Path==NULL || strlen(Path) < 1) return E_FAIL;

	bool slashed = (Path[strlen(Path)-1] == '\\' || Path[strlen(Path)-1] == '/');

	char* buffer = new char [strlen(Path) + 1 + (slashed?0:1)];
	if(buffer==NULL) return E_FAIL;

	strcpy(buffer, Path);
	if(!slashed) strcat(buffer, "\\");
	//CBPlatform::strlwr(buffer);

	switch(Type)
	{
		case PATH_SINGLE: m_SinglePaths.Add(buffer); break;
		case PATH_PACKAGE: m_PackagePaths.Add(buffer); break;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::ReloadPaths()
{
	// delete registered paths
	for(int i=0; i<m_SinglePaths.GetSize(); i++)
		delete [] m_SinglePaths[i];
	m_SinglePaths.RemoveAll();

	for(int i=0; i<m_PackagePaths.GetSize(); i++)
		delete [] m_PackagePaths[i];
	m_PackagePaths.RemoveAll();

	return InitPaths();
}


#define TEMP_BUFFER_SIZE 32768
//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::InitPaths()
{
	RestoreCurrentDir();

	AnsiString pathList;
	int numPaths;

	// single files paths	
	pathList = Game->m_Registry->ReadString("Resource", "CustomPaths", "");
	numPaths = CBUtils::StrNumEntries(pathList.c_str(), ';');

	for(int i = 0; i < numPaths; i++)
	{
		char* path = CBUtils::StrEntry(i, pathList.c_str(), ';');
		if(path && strlen(path) > 0)
		{
			AddPath(PATH_SINGLE, path);
		}
		SAFE_DELETE_ARRAY(path);
	}
	AddPath(PATH_SINGLE, ".\\");


	// package files paths
	AddPath(PATH_PACKAGE, "./");

#ifdef __APPLE__
	// search .app path and Resources dir in the bundle
	CFURLRef appUrlRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
	CFStringRef macPath = CFURLCopyFileSystemPath(appUrlRef, kCFURLPOSIXPathStyle);
	const char* pathPtr = CFStringGetCStringPtr(macPath, CFStringGetSystemEncoding());
	
#ifdef __IPHONE__
	AddPath(PATH_PACKAGE, pathPtr);
	AddPath(PATH_SINGLE, pathPtr);	
#else
	char bundlePath[MAX_PATH];

	sprintf(bundlePath, "%s/../", pathPtr);	
	AddPath(PATH_PACKAGE, bundlePath);
	AddPath(PATH_SINGLE, bundlePath);

	sprintf(bundlePath, "%s/Contents/Resources/", pathPtr);	
	AddPath(PATH_PACKAGE, bundlePath);
	AddPath(PATH_SINGLE, bundlePath);


	CFRelease(appUrlRef);
	CFRelease(macPath);
#endif
#elif __ANDROID__
	char androidPath[1024];
	android_getGamePackagePath(androidPath, 1024);
	AddPath(PATH_PACKAGE, androidPath);
	android_getGameFilePath(androidPath, 1024);
	AddPath(PATH_SINGLE, androidPath);
#endif
	

	pathList = Game->m_Registry->ReadString("Resource", "PackagePaths", "");
	numPaths = CBUtils::StrNumEntries(pathList.c_str(), ';');

	for(int i = 0; i < numPaths; i++)
	{
		char* path = CBUtils::StrEntry(i, pathList.c_str(), ';');
		if(path && strlen(path) > 0)
		{
			AddPath(PATH_PACKAGE, path);
		}
		SAFE_DELETE_ARRAY(path);
	}
	AddPath(PATH_PACKAGE, "data");

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::RegisterPackages()
{
	RestoreCurrentDir();

	Game->LOG(0, "Scanning packages...");

	AnsiString mask = AnsiString("*.") + AnsiString(PACKAGE_EXTENSION);
	
	for (int i = 0; i < m_PackagePaths.GetSize(); i++)
	{
		AnsiString fullPath = PathUtil::GetAbsolutePath(m_PackagePaths[i]);
		if (!CBPlatform::DirectoryExists(fullPath.c_str())) continue;

		AnsiStringList files;
		PathUtil::GetFilesInDirectory(fullPath, mask, files);

		for (AnsiStringList::iterator it = files.begin(); it != files.end(); ++it)
		{
			if (!IsValidPackage(PathUtil::Combine(fullPath, (*it)))) continue;
			RegisterPackage(fullPath.c_str(), (*it).c_str());
		}
	}
	Game->LOG(0, "  Registered %d files in %d package(s)", m_Files.size(), m_Packages.GetSize());

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::RegisterPackage(const AnsiString& path, const AnsiString& name, bool searchSignature)
{
    AnsiString fileName = PathUtil::Combine(path, name);

	FILE* f = fopen(fileName.c_str(), "rb");
	if(!f)
	{
		Game->LOG(0, "  Error opening package file '%s'. Ignoring.", fileName.c_str());
		return S_OK;
	}

	DWORD AbosulteOffset = 0;
	bool BoundToExe = false;

	if(searchSignature)
	{
		DWORD Offset;
		if(!FindPackageSignature(f, &Offset))
		{
			fclose(f);
			return S_OK;
		}
		else
		{
			fseek(f, Offset, SEEK_SET);
			AbosulteOffset = Offset;
			BoundToExe = true;
		}
	}

	TPackageHeader hdr;
	fread(&hdr, sizeof(TPackageHeader), 1, f);
	if(hdr.Magic1 != PACKAGE_MAGIC_1 || hdr.Magic2 != PACKAGE_MAGIC_2 || hdr.PackageVersion > PACKAGE_VERSION)
	{
		Game->LOG(0, "  Invalid header in package file '%s'. Ignoring.", fileName.c_str());
		fclose(f);
		return S_OK;
	}

	if(hdr.PackageVersion != PACKAGE_VERSION)
	{
		Game->LOG(0, "  Warning: package file '%s' is outdated.", fileName.c_str());
	}

	// new in v2
	if(hdr.PackageVersion==PACKAGE_VERSION)
	{
		DWORD DirOffset;
		fread(&DirOffset, sizeof(DWORD), 1, f);
		DirOffset+=AbosulteOffset;
		fseek(f, DirOffset, SEEK_SET);
	}

	for(int i=0; i<hdr.NumDirs; i++)
	{
		CBPackage* pkg = new CBPackage(Game);
		if(!pkg) return E_FAIL;

		pkg->m_BoundToExe = BoundToExe;

		// read package info
		BYTE NameLength;
		fread(&NameLength, sizeof(BYTE), 1, f);
		pkg->m_Name = new char[NameLength];
		fread(pkg->m_Name, NameLength, 1, f);
		fread(&pkg->m_CD, sizeof(BYTE), 1, f);
		pkg->m_Priority = hdr.Priority;

		if(!hdr.MasterIndex) pkg->m_CD = 0; // override CD to fixed disk
		m_Packages.Add(pkg);


		// read file entries
		DWORD NumFiles;
		fread(&NumFiles, sizeof(DWORD), 1, f);

		for(int j=0; j<NumFiles; j++)
		{
			char* Name;
			DWORD Offset, Length, CompLength, Flags, TimeDate1, TimeDate2;

			fread(&NameLength, sizeof(BYTE), 1, f);
			Name = new char[NameLength];
			fread(Name, NameLength, 1, f);
			
			// v2 - xor name
			if(hdr.PackageVersion==PACKAGE_VERSION)
			{
				for(int k=0; k<NameLength; k++)
				{
					((BYTE*)Name)[k] ^= 'D';
				}
			}

			// some old version of ProjectMan writes invalid directory entries
			// so at least prevent strupr from corrupting memory
			Name[NameLength - 1] = '\0';


			CBPlatform::strupr(Name);

			fread(&Offset, sizeof(DWORD), 1, f);
			Offset+=AbosulteOffset;
			fread(&Length, sizeof(DWORD), 1, f);
			fread(&CompLength, sizeof(DWORD), 1, f);
			fread(&Flags, sizeof(DWORD), 1, f);

			if(hdr.PackageVersion==PACKAGE_VERSION)
			{
				fread(&TimeDate1, sizeof(DWORD), 1, f);
				fread(&TimeDate2, sizeof(DWORD), 1, f);
			}

			m_FilesIter = m_Files.find(Name);
			if (m_FilesIter == m_Files.end())
			{
				CBFileEntry* file = new CBFileEntry(Game);
				file->m_Package = pkg;
				file->m_Offset = Offset;
				file->m_Length = Length;
				file->m_CompressedLength = CompLength;
				file->m_Flags = Flags;

				m_Files[Name] = file;
			}
			else
			{
				// current package has lower CD number or higher priority, than the registered
				if(pkg->m_CD < m_FilesIter->second->m_Package->m_CD || pkg->m_Priority > m_FilesIter->second->m_Package->m_Priority)
				{
					m_FilesIter->second->m_Package = pkg;
					m_FilesIter->second->m_Offset = Offset;
					m_FilesIter->second->m_Length = Length;
					m_FilesIter->second->m_CompressedLength = CompLength;
					m_FilesIter->second->m_Flags = Flags;					
				}
			}
			delete [] Name;
		}
	}


	fclose(f);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CBFileManager::IsValidPackage(const AnsiString& fileName) const
{
	AnsiString plainName = PathUtil::GetFileNameWithoutExtension(fileName);

	// check for device-type specific packages
	if (StringUtil::StartsWith(plainName, "xdevice_", true))
	{				
		return StringUtil::CompareNoCase(plainName, "xdevice_" + Game->GetDeviceType());
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
FILE* CBFileManager::OpenPackage(char *Name)
{
	RestoreCurrentDir();

	FILE* ret = NULL;
	char Filename[MAX_PATH];

	for(int i=0; i<m_PackagePaths.GetSize(); i++)
	{
		sprintf(Filename, "%s%s.%s", m_PackagePaths[i], Name, PACKAGE_EXTENSION);
		ret = fopen(Filename, "rb");
		if(ret!=NULL) return ret;
	}
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
FILE* CBFileManager::OpenSingleFile(char* Name)
{
	RestoreCurrentDir();

	FILE* ret = NULL;
	char Filename[MAX_PATH];

	for(int i=0; i<m_SinglePaths.GetSize(); i++)
	{
		sprintf(Filename, "%s%s", m_SinglePaths[i], Name);
		ret = fopen(Filename, "rb");
		if(ret!=NULL) return ret;
	}

	// didn't find in search paths, try to open directly
	return fopen(Name, "rb");
}


//////////////////////////////////////////////////////////////////////////
bool CBFileManager::GetFullPath(char *Filename, char *Fullname)
{
	RestoreCurrentDir();

	FILE* f = NULL;
	bool found = false;

	for(int i=0; i<m_SinglePaths.GetSize(); i++)
	{
		sprintf(Fullname, "%s%s", m_SinglePaths[i], Filename);
		f = fopen(Fullname, "rb");
		if(f)
		{
			fclose(f);
			found = true;
			break;
		}
	}

	if(!found)
	{
		f = fopen(Filename, "rb");
		if(f)
		{
			fclose(f);
			found = true;
			strcpy(Fullname, Filename);
		}
	}

	return found;
}


//////////////////////////////////////////////////////////////////////////
CBFileEntry* CBFileManager::GetPackageEntry(const char *Filename)
{
	char* upc_name = new char[strlen(Filename)+1];
	strcpy(upc_name, Filename);
	CBPlatform::strupr(upc_name);

	CBFileEntry* ret=NULL;
	m_FilesIter = m_Files.find(upc_name);
	if(m_FilesIter != m_Files.end()) ret = m_FilesIter->second;

	delete [] upc_name;

	return ret;
}


//////////////////////////////////////////////////////////////////////////
CBFile* CBFileManager::OpenFile(const char *Filename, bool AbsPathWarning)
{
	if (strcmp(Filename, "") == 0) return NULL;
	//Game->LOG(0, "open file: %s", Filename);
#ifdef __WIN32__
	if(Game->m_DEBUG_DebugMode && Game->m_DEBUG_AbsolutePathWarning && AbsPathWarning)
	{
		char Drive[_MAX_DRIVE];
		_splitpath(Filename, Drive, NULL, NULL, NULL);
		if(Drive[0]!='\0')
		{
			Game->LOG(0, "WARNING: Referencing absolute path '%s'. The game will NOT work on another computer.", Filename);
		}
	}
#endif

	CBFile* File = OpenFileRaw(Filename);
	if(File) m_OpenFiles.Add(File);
	return File;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::CloseFile(CBFile *File)
{
	for(int i=0; i<m_OpenFiles.GetSize(); i++)
	{
		if(m_OpenFiles[i]==File)
		{
			m_OpenFiles[i]->Close();
			delete m_OpenFiles[i];
			m_OpenFiles.RemoveAt(i);
			return S_OK;
		}
	}
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
CBFile* CBFileManager::OpenFileRaw(const char *Filename)
{
	RestoreCurrentDir();

	if(CBPlatform::strnicmp(Filename, "savegame:", 9)==0)
	{
		CBSaveThumbFile* SaveThumbFile = new CBSaveThumbFile(Game);
		if(SUCCEEDED(SaveThumbFile->Open(Filename))) return SaveThumbFile;
		else
		{
			delete SaveThumbFile;
			return NULL;
		}
	}

	CBDiskFile* DiskFile = new CBDiskFile(Game);
	if(SUCCEEDED(DiskFile->Open(Filename))) return DiskFile;

	delete DiskFile;

	CBPkgFile* PkgFile = new CBPkgFile(Game);
	if(SUCCEEDED(PkgFile->Open(Filename))) return PkgFile;

	delete PkgFile;

	CBResourceFile* ResFile = new CBResourceFile(Game);
	if(SUCCEEDED(ResFile->Open(Filename))) return ResFile;

	delete ResFile;

	return NULL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::RestoreCurrentDir()
{
	if(!m_BasePath) return S_OK;
	else
	{
		if(!chdir(m_BasePath)) return S_OK;
		else return E_FAIL;
	}
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFileManager::SetBasePath(char *Path)
{
	Cleanup();

	if(Path)
	{
		m_BasePath = new char[strlen(Path)+1];
		strcpy(m_BasePath, Path);
	}

	InitPaths();
	RegisterPackages();
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
bool CBFileManager::FindPackageSignature(FILE *f, DWORD *Offset)
{
	BYTE buf[32768];
	
	BYTE Signature[8];
	((DWORD*)Signature)[0] = PACKAGE_MAGIC_1;
	((DWORD*)Signature)[1] = PACKAGE_MAGIC_2;

	fseek(f, 0, SEEK_END);
	DWORD FileSize = ftell(f);

	int StartPos = 1024*1024;
	
	int BytesRead = StartPos;

	while(BytesRead<FileSize-16)
	{
		int ToRead = MIN(32768, FileSize - BytesRead);
		fseek(f, StartPos, SEEK_SET);
		int ActuallyRead = fread(buf, 1, ToRead, f);
		if(ActuallyRead != ToRead) return false;

		for (int i = 0; i<ToRead-8; i++)
			if(!memcmp(buf + i, Signature, 8))
			{
				*Offset =  StartPos + i;
				return true;
			}

		BytesRead = BytesRead + ToRead - 16;
		StartPos = StartPos + ToRead - 16;

	}
	return false;

}
