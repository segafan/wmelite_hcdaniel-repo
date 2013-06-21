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
#include "BPackage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
CBPackage::CBPackage(CBGame* inGame):CBBase(inGame)
{
	m_File = NULL;
	m_Name = NULL;
	m_CD = 0;
	m_Priority = 0;
	m_BoundToExe = false;
}


//////////////////////////////////////////////////////////////////////////
CBPackage::~CBPackage()
{
	if(m_Name) delete [] m_Name;
	CloseFilePointer(m_File);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBPackage::Open()
{
	if (m_File) return S_OK;
	else
	{
		m_File = GetFilePointer();
		return m_File ? S_OK : E_FAIL;
	}
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBPackage::Close()
{
	if(m_File) ops->file_close(m_File);
	m_File = NULL;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBPackage::Read(FILEHANDLE file, DWORD offset, BYTE* buffer, DWORD size)
{
	HRESULT ret;
	if (FAILED(ret = Open())) return ret;
	else
	{
		if (ops->file_seek(file, offset, SEEK_SET)) return E_FAIL;
		if (ops->file_read((char *) buffer, size, file) != 1) return E_FAIL;
		else return S_OK;
	}
}

//////////////////////////////////////////////////////////////////////////
FILEHANDLE CBPackage::GetFilePointer()
{
	FILEHANDLE file = Game->m_FileManager->OpenPackage(m_Name, &ops);
	if (!file)
	{
		Game->m_FileManager->RequestCD(m_CD, m_Name, "");
		file = Game->m_FileManager->OpenPackage(m_Name, &ops);
	}
	return file;
}

//////////////////////////////////////////////////////////////////////////
void CBPackage::CloseFilePointer(FILEHANDLE file)
{
	if (file) ops->file_close(file);
	file = NULL;
}
