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
#include "BDynBuffer.h"


//////////////////////////////////////////////////////////////////////////
CBDynBuffer::CBDynBuffer(CBGame* inGame, DWORD InitSize, DWORD GrowBy):CBBase(inGame)
{
	m_Buffer = NULL;
	m_Size = 0;
	m_RealSize = 0;

	m_Offset = 0;
	m_InitSize = InitSize;
	m_GrowBy = GrowBy;

	m_Initialized = false;
}


//////////////////////////////////////////////////////////////////////////
CBDynBuffer::~CBDynBuffer()
{
	Cleanup();
}


//////////////////////////////////////////////////////////////////////////
void CBDynBuffer::Cleanup()
{
	if(m_Buffer) free(m_Buffer);
	m_Buffer = NULL;
	m_Size = 0;
	m_RealSize = 0;
	m_Offset = 0;
	m_Initialized = false;
}


//////////////////////////////////////////////////////////////////////////
DWORD CBDynBuffer::GetSize()
{
	return m_Size;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDynBuffer::Init(DWORD InitSize)
{
	Cleanup();

	if(InitSize==0) InitSize = m_InitSize;

	m_Buffer = (BYTE*)malloc(InitSize);
	if(!m_Buffer){
		Game->LOG(0, "CBDynBuffer::Init - Error allocating %d bytes", InitSize);
			return E_FAIL;
	}

	m_RealSize = InitSize;
	m_Initialized = true;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDynBuffer::PutBytes(BYTE *Buffer, DWORD Size)
{
	if(!m_Initialized) Init();

	while(m_Offset + Size > m_RealSize){
		m_RealSize += m_GrowBy;
		m_Buffer = (BYTE*)realloc(m_Buffer, m_RealSize);
		if(!m_Buffer){
			Game->LOG(0, "CBDynBuffer::PutBytes - Error reallocating buffer to %d bytes", m_RealSize);
			return E_FAIL;
		}
	}

	memcpy(m_Buffer+m_Offset, Buffer, Size);
	m_Offset+=Size;
	m_Size+=Size;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDynBuffer::GetBytes(BYTE *Buffer, DWORD Size)
{
	if(!m_Initialized) Init();

	if(m_Offset+Size>m_Size){
		Game->LOG(0, "CBDynBuffer::GetBytes - Buffer underflow");
		return E_FAIL;
	}

	memcpy(Buffer, m_Buffer+m_Offset, Size);
	m_Offset+=Size;
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CBDynBuffer::PutDWORD(DWORD Val)
{
	PutBytes((BYTE*)&Val, sizeof(DWORD));
}


//////////////////////////////////////////////////////////////////////////
DWORD CBDynBuffer::GetDWORD()
{
	DWORD ret;
	GetBytes((BYTE*)&ret, sizeof(DWORD));
	return ret;
}


//////////////////////////////////////////////////////////////////////////
void CBDynBuffer::PutString(const char* Val)
{
	if(!Val) PutString("(null)");
	else{
		PutDWORD(strlen(Val)+1);
		PutBytes((BYTE*)Val, strlen(Val)+1);
	}
}


//////////////////////////////////////////////////////////////////////////
char* CBDynBuffer::GetString()
{
	DWORD len = GetDWORD();
	char* ret = (char*)(m_Buffer+m_Offset);
	m_Offset+=len;

	if(!strcmp(ret, "(null)")) return NULL;
	else return ret;
}


//////////////////////////////////////////////////////////////////////////
void CBDynBuffer::PutText(LPCSTR fmt, ...)
{	
	va_list va;

	va_start(va, fmt);
	PutTextForm(fmt, va);
	va_end(va);
	
}


//////////////////////////////////////////////////////////////////////////
void CBDynBuffer::PutTextIndent(int Indent, LPCSTR fmt, ...)
{	
	va_list va;
	
	PutText("%*s", Indent, "");

	va_start(va, fmt);
	PutTextForm(fmt, va);
	va_end(va);
}


//////////////////////////////////////////////////////////////////////////
void CBDynBuffer::PutTextForm(const char *format, va_list argptr)
{
	char buff[32768];
	vsprintf(buff, format, argptr);
	PutBytes((BYTE*)buff, strlen(buff));
}