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
#include "BResourceFile.h"
#include "BResources.h"


//////////////////////////////////////////////////////////////////////////
CBResourceFile::CBResourceFile(CBGame* inGame):CBFile(inGame)
{
	m_Data = NULL;
}


//////////////////////////////////////////////////////////////////////////
CBResourceFile::~CBResourceFile()
{
	Close();
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBResourceFile::Open(const char* Filename)
{
	Close();

	if (CBResources::GetFile(Filename, m_Data, m_Size))
	{
		m_Pos = 0;
		return S_OK;
	}
	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBResourceFile::Close()
{
	m_Data = NULL;
	m_Pos = 0;
	m_Size = 0;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBResourceFile::Read(void *Buffer, DWORD Size)
{
	if(!m_Data || m_Pos + Size > m_Size) return E_FAIL;

	memcpy(Buffer, (BYTE*)m_Data+m_Pos, Size);
	m_Pos+=Size;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBResourceFile::Seek(DWORD Pos, TSeek Origin)
{
	if(!m_Data) return E_FAIL;

	DWORD NewPos=0;

	switch (Origin)
	{
		case SEEK_TO_BEGIN:   NewPos = Pos;          break;
		case SEEK_TO_END:     NewPos = m_Size + Pos; break;
		case SEEK_TO_CURRENT: NewPos = m_Pos + Pos;  break;
	}

	if( NewPos<0 || NewPos > m_Size) return E_FAIL;
	else m_Pos = NewPos;

	return S_OK;
}
