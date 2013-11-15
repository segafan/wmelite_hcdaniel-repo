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

#ifndef __WmeBFileManager_H__
#define __WmeBFileManager_H__


#include <map>
#include "coll_templ.h"
#include "FileOperations.h"

class CBFileManager:CBBase
{
public:	
	bool FindPackageSignature(generic_file_ops *ops, FILEHANDLE f, DWORD* Offset);
	HRESULT Cleanup();
	HRESULT SetBasePath(char* Path);
	HRESULT RestoreCurrentDir();
	char* m_BasePath;
	bool GetFullPath(char* Filename, char* Fullname);
	CBFile* OpenFileRaw(const char* Filename);
	HRESULT CloseFile(CBFile* File);
	CBFile* OpenFile(const char* Filename, bool AbsPathWarning=true);
	CBFileEntry* GetPackageEntry(const char* Filename);
	FILEHANDLE OpenSingleFile(char* Name);
	FILEHANDLE OpenPackage(char* Name, generic_file_ops **ops);
	HRESULT RegisterPackages();	
	HRESULT InitPaths();
	HRESULT ReloadPaths();
	typedef enum{
		PATH_PACKAGE, PATH_SINGLE
	} TPathType;
	HRESULT AddPath(TPathType Type, const char* Path);
	HRESULT RequestCD(int CD, char* PackageFile, char* Filename);
	HRESULT SaveFile(char* Filename, BYTE* Buffer, DWORD BufferSize, bool Compressed=false, BYTE* PrefixBuffer=NULL, DWORD PrefixSize=0);
	BYTE* ReadWholeFile(const char* Filename, DWORD* Size=NULL, bool MustExist=true);
	CBFileManager(CBGame* inGame=NULL);
	virtual ~CBFileManager();
	CBArray<char*, char*> m_SinglePaths;
	CBArray<char*, char*> m_PackagePaths;
	CBArray<CBPackage*, CBPackage*> m_Packages;
	CBArray<CBFile*, CBFile*> m_OpenFiles;

	map<string, CBFileEntry*> m_Files;
private:
	HRESULT RegisterPackage(const AnsiString& path, const AnsiString& name, bool searchSignature = false);
	map<string, CBFileEntry*>::iterator m_FilesIter;
	bool IsValidPackage(const AnsiString& fileName) const;
	
};

#endif
