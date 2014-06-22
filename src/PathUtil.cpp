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
#include <algorithm>
#include <fstream>
#include "PathUtil.h"
#include "StringUtil.h"
#include "DirectoryOperations.h"


#ifdef __WIN32__
#	include <shlobj.h>
#	include <io.h>
#else
#   include <dirent.h>
#endif

#ifdef __MACOSX__
#	include <CoreServices/CoreServices.h>
#endif

#ifdef __IPHONEOS__
#	include "ios_utils.h"
#endif

#ifdef __ANDROID__
#	include "android/android.h"
#elif  __LINUX__
#	include <pwd.h>
#endif


const AnsiString assetIdentifier = "asset://";
const AnsiString obbPlainIdentifier = "obbplain://";
const AnsiString obbMountIdentifier = "obbmount://";

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::AppendSlashToPlainDir(const AnsiString& path)
{
	const char *cpath = path.c_str();
	bool slashed = (path[strlen(cpath)-1] == '\\' || path[strlen(cpath)-1] == '/');

	char* buffer = new char [strlen(cpath) + 1 + (slashed ? 0 : 1)];
	if(buffer==NULL) return NULL;

	strcpy(buffer, cpath);
	if(!slashed) strcat(buffer, "\\");
	//CBPlatform::strlwr(buffer);

	return AnsiString(buffer);
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::UnifySeparators(const AnsiString& path)
{
	AnsiString newPath = path;

	std::replace(newPath.begin(), newPath.end(), L'\\', L'/');
	return newPath;
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::NormalizeFileName(const AnsiString& path)
{
	AnsiString newPath = UnifySeparators(path);
	StringUtil::ToLowerCase(newPath);
	return newPath;
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::Combine(const AnsiString& path1, const AnsiString& path2)
{
	AnsiString newPath1 = UnifySeparators(path1);
	AnsiString newPath2 = UnifySeparators(path2);

	if (!StringUtil::EndsWith(newPath1, "/", true) && !StringUtil::StartsWith(newPath2, "/", true))
		newPath1 += "/";

	return newPath1 + newPath2;
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::GetDirectoryName(const AnsiString& path)
{
	AnsiString newPath = UnifySeparators(path);
	
	size_t pos = newPath.find_last_of(L'/');
	
	if (pos == AnsiString::npos) return "";
	else return newPath.substr(0, pos + 1);
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::GetFileName(const AnsiString& path)
{
	AnsiString newPath = UnifySeparators(path);

	size_t pos = newPath.find_last_of(L'/');

	if (pos == AnsiString::npos) return path;
	else return newPath.substr(pos + 1);
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::GetFileNameWithoutExtension(const AnsiString& path)
{
	AnsiString fileName = GetFileName(path);

	size_t pos = fileName.find_last_of('.');

	if (pos == AnsiString::npos) return fileName;
	else return fileName.substr(0, pos);
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::GetExtension(const AnsiString& path)
{
	AnsiString fileName = GetFileName(path);

	size_t pos = fileName.find_last_of('.');

	if (pos == AnsiString::npos) return "";
	else return fileName.substr(pos);
}


//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::GetSafeLogFileName()
{
#ifndef __ANDROID__

	AnsiString logFileName = GetUserDirectory();

#ifdef __WIN32__
	char moduleName[MAX_PATH];
	::GetModuleFileName(NULL, moduleName, MAX_PATH);
	
	AnsiString fileName = GetFileNameWithoutExtension(moduleName) + ".log";
	fileName = Combine("/Wintermute Engine/Logs/", fileName);
	logFileName = Combine(logFileName, fileName);
	
#else
	// !PORTME	
	logFileName = Combine(logFileName, "/Wintermute Engine/wme.log");
#endif

#else
	char androidPath[1024];
	android_getLogFileDirectory(androidPath, 1024);
	// use a storage path in Android that is easily available
	// typically that would be the external storage path
	AnsiString logFileName = Combine(androidPath, "/wme.log");

#endif

	CreateDirectory(GetDirectoryName(logFileName));
	return logFileName;
}

//////////////////////////////////////////////////////////////////////////
bool PathUtil::CreateDirectory(const AnsiString& path)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool PathUtil::MatchesMask(const AnsiString& fileName, const AnsiString& mask)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool PathUtil::FileExists(const char* fileName)
{
	FILEHANDLE file;
	generic_file_ops *ops = GetFileAccessMethod(fileName);

	file = ops->file_open(fileName, "rb");
	if (!file) return false;

	ops->file_close(file);
	return true;
}

//////////////////////////////////////////////////////////////////////////
bool PathUtil::DirectoryExists(const char* path)
{
	bool ret = false;
#ifdef _WIN32
	DWORD attr = GetFileAttributes(path);
	ret = (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
#else
	DIRHANDLE dir;
	generic_directory_ops *ops = GetDirectoryAccessMethod(path);
	dir = ops->dir_open(path);
	if (dir != NULL)
	{
		ret = true;
		ops->dir_close(dir);
	}
#endif
	return ret;
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::GetUserDirectory()
{
	AnsiString userDir = "./";

#ifdef __WIN32__
	char buffer[MAX_PATH];
	buffer[0] = '\0';
	LPITEMIDLIST pidl = NULL;
	LPMALLOC pMalloc;
	if(SUCCEEDED(SHGetMalloc(&pMalloc)))
	{
		SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl);
		if(pidl)
		{
			SHGetPathFromIDList(pidl, buffer);
		}
		pMalloc->Free(pidl);
		userDir = AnsiString(buffer);
	}
#elif __MACOSX__
	FSRef fileRef;
	OSStatus error = FSFindFolder(kUserDomain, kApplicationSupportFolderType, true, &fileRef);
	if (error == noErr)
	{
		char buffer[MAX_PATH];
		error = FSRefMakePath(&fileRef, (UInt8*)buffer, sizeof(buffer));
		if (error == noErr)
			userDir = buffer;

	}
#elif __IPHONEOS__
	char path[MAX_PATH];
	IOS_GetDataDir(path);
	userDir = AnsiString(path);
#elif __ANDROID__
	char androidPath[1024];
	android_getPrivateFilesPath(androidPath, 1024);
	userDir = AnsiString(androidPath);

#elif __LINUX__
	char* strHome = getenv("HOME");

	if(strHome != NULL) {
		userDir = AnsiString(strHome);
	}
	else {
		struct passwd *pw = getpwuid(getuid());
		if(pw != NULL) {
			userDir = AnsiString(pw->pw_dir);
		}
	}
#endif
	
	return userDir;
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::GetLocalSettingsDirectory()
{
	AnsiString localSettingsDir;

#ifndef __ANDROID__

	localSettingsDir =  ".";

#else

	char androidPath[1024];
	android_getLocalSettingsPath(androidPath, 1024);
	localSettingsDir = AnsiString(androidPath);

#endif

	return localSettingsDir;
}

//////////////////////////////////////////////////////////////////////////
AnsiString PathUtil::GetAbsolutePath(const AnsiString& path)
{
#ifdef _WIN32
	char fullPath[MAX_PATH];
	_fullpath(fullPath, path.c_str(), MAX_PATH);
#else
	char fullPath[32768]; // UNIX paths can be longer

#ifdef __ANDROID__
	if (StringUtil::StartsWith(path, assetIdentifier, false) == true) {
		return path;
	}
	if (StringUtil::StartsWith(path, obbPlainIdentifier, false) == true) {
		return path;
	}
	if (StringUtil::StartsWith(path, obbMountIdentifier, false) == true) {
		return path;
	}
#endif

	realpath(path.c_str(), fullPath);

#endif
	return AnsiString(fullPath);
}

//////////////////////////////////////////////////////////////////////////
void PathUtil::GetFilesInDirectory(const AnsiString& path, const AnsiString& mask, AnsiStringList& files)
{
	AnsiString fileSpec = PathUtil::Combine(path, mask);

#ifdef _WIN32
	struct _finddata_t c_file;
	intptr_t hFile;

	if ((hFile = _findfirst(fileSpec.c_str(), &c_file)) == -1L) return;
	else
	{
		do
		{
			if (!(c_file.attrib & _A_SUBDIR))
			{
				files.push_back(c_file.name);
			}
		} while (_findnext(hFile, &c_file ) == 0);
		_findclose(hFile);
	}
#else
    DIRHANDLE dir;
    char* entry;
    generic_directory_ops *ops = GetDirectoryAccessMethod(path);
    
    dir = ops->dir_open(path.c_str());
    if (dir)
    {
        while ((entry = ops->dir_find(dir)) != NULL)
        {
            if (CBUtils::MatchesPattern(mask.c_str(), entry))
            {
                files.push_back(entry);
            }
        }
        ops->dir_close(dir);
    }
#endif
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
generic_directory_ops* PathUtil::GetDirectoryAccessMethod(const AnsiString &path)
{
#ifdef _WIN32
	return get_directory_operations(DIR_ACCESS_VARIANT_PLAIN);
#else
	dir_access_variant variant = DIR_ACCESS_VARIANT_PLAIN;

	if (StringUtil::StartsWith(path, assetIdentifier, false) == true)
	{
		variant = DIR_ACCESS_VARIANT_ANDROID_ASSET;
	}
	if (StringUtil::StartsWith(path, obbPlainIdentifier, false) == true)
	{
		variant = DIR_ACCESS_VARIANT_ANDROID_OBB_PLAIN;
	}
	if (StringUtil::StartsWith(path, obbMountIdentifier, false) == true)
	{
		variant = DIR_ACCESS_VARIANT_ANDROID_OBB_MOUNT;
	}

	return get_directory_operations(variant);
#endif
}

generic_file_ops* PathUtil::GetFileAccessMethod(const AnsiString &path)
{
#ifdef _WIN32
	return get_file_operations(FILE_ACCESS_VARIANT_PLAIN);
#else
	file_access_variant variant = FILE_ACCESS_VARIANT_PLAIN;

	if (StringUtil::StartsWith(path, assetIdentifier, false) == true)
	{
		variant = FILE_ACCESS_VARIANT_ANDROID_ASSET;
	}
	if (StringUtil::StartsWith(path, obbPlainIdentifier, false) == true)
	{
		variant = FILE_ACCESS_VARIANT_ANDROID_OBB_PLAIN;
	}
	if (StringUtil::StartsWith(path, obbMountIdentifier, false) == true)
	{
		variant = FILE_ACCESS_VARIANT_ANDROID_OBB_MOUNT;
	}

	return get_file_operations(variant);
#endif
}
