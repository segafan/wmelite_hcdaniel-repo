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

#ifndef __WmeBPkgFile_H__
#define __WmeBPkgFile_H__


#include "BFile.h"
#include "zlib.h"	// Added by ClassView

#define COMPRESSED_BUFFER_SIZE 4096

class CBPkgFile : public CBFile  
{
public:	
	CBPkgFile(CBGame* inGame);
	virtual ~CBPkgFile();
	virtual HRESULT Seek(DWORD Pos, TSeek Origin=SEEK_TO_BEGIN);
	virtual HRESULT Read(void* Buffer, DWORD Size);
	virtual HRESULT Close();
    virtual HRESULT Open(const char* Filename);
private:
	bool m_InflateInit;
	HRESULT SeekToPos(DWORD NewPos);
	bool m_Compressed;
	CBFileEntry* m_FileEntry;
	z_stream m_Stream;
	BYTE m_CompBuffer[COMPRESSED_BUFFER_SIZE];
	FILE* m_File;
};

#endif
