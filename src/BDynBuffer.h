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

#ifndef __WmeBDynBuffer_H__
#define __WmeBDynBuffer_H__


#include "BBase.h"

class CBDynBuffer : public CBBase  
{
public:
	bool m_Initialized;
	void PutText(LPCSTR fmt, ...);
	void PutTextIndent(int Indent, LPCSTR fmt, ...);	
	DWORD GetDWORD();
	void PutDWORD(DWORD Val);
	char* GetString();
	void PutString(const char* Val);
	HRESULT GetBytes(BYTE* Buffer, DWORD Size);
	HRESULT PutBytes(BYTE* Buffer, DWORD Size);
	DWORD GetSize();
	HRESULT Init(DWORD InitSize=0);
	void Cleanup();
	DWORD m_Size;
	BYTE* m_Buffer;
	CBDynBuffer(CBGame* inGame, DWORD InitSize=1000, DWORD GrowBy=1000);
	virtual ~CBDynBuffer();

private:
	DWORD m_RealSize;
	DWORD m_GrowBy;
	DWORD m_InitSize;
	DWORD m_Offset;
	void PutTextForm(const char *format, va_list argptr);
};

#endif
