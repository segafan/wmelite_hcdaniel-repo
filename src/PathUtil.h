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

#ifndef __WmePathUtils_H__
#define __WmePathUtils_H__

#include "DirectoryOperations.h"
#include "FileOperations.h"

	class PathUtil
	{
	public:
		static AnsiString UnifySeparators(const AnsiString& path);
		static AnsiString NormalizeFileName(const AnsiString& path);
		static AnsiString Combine(const AnsiString& path1, const AnsiString& path2);
		static AnsiString GetDirectoryName(const AnsiString& path);
		static AnsiString GetFileName(const AnsiString& path);
		static AnsiString GetFileNameWithoutExtension(const AnsiString& path);
		static AnsiString GetExtension(const AnsiString& path);
		static AnsiString GetAbsolutePath(const AnsiString& path);
		static bool CreateDirectory(const AnsiString& path);
		static bool MatchesMask(const AnsiString& fileName, const AnsiString& mask);

		static bool FileExists(const AnsiString& fileName);

		static AnsiString GetSafeLogFileName();
		static AnsiString GetUserDirectory();

		static void GetFilesInDirectory(const AnsiString& path, const AnsiString& mask, AnsiStringList& files);
		static generic_directory_ops* GetDirectoryAccessMethod(const AnsiString &path);
		static generic_file_ops* GetFileAccessMethod(const AnsiString &path);
	};

#endif // __WmePathUtils_H__
