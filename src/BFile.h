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

#ifndef __WmeBFile_H__
#define __WmeBFile_H__


#include "BBase.h"

class CBFile : public CBBase
{
public:
	DWORD m_Pos;
	DWORD m_Size;
	virtual DWORD GetSize(){ return m_Size; };
	virtual DWORD GetPos() { return m_Pos; };
	virtual HRESULT Seek(DWORD Pos, TSeek Origin=SEEK_TO_BEGIN)=0;
	virtual HRESULT Read(void* Buffer, DWORD Size)=0;
	virtual HRESULT Close()=0;
	virtual HRESULT Open(const char* Filename)=0;
	virtual bool IsEOF();
	CBFile(CBGame* inGame);
	virtual ~CBFile();
};

#endif
