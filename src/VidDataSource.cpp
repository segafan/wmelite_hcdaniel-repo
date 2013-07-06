/*
This file is part of WME Lite.
http://dead-code.org/redir.php?target=wmelite

Copyright (c) 2013 Jan Nedoma

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
#include "VidDataSource.h"
#include <theoraplayer/TheoraPlayer.h>


//////////////////////////////////////////////////////////////////////////
CVidDataSource::CVidDataSource(CBGame* inGame, CBFile* file, const char* fileName)
{
	m_Game = inGame;
	m_Filename = fileName;
	m_File = file;
}

//////////////////////////////////////////////////////////////////////////
CVidDataSource::~CVidDataSource()
{
}


//////////////////////////////////////////////////////////////////////////
int CVidDataSource::read(void* output, int nBytes)
{
	if (m_File)
	{
		DWORD oldPos = m_File->GetPos();
		m_File->Read(output, nBytes);
		return m_File->GetPos() - oldPos;
	}
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
std::string CVidDataSource::repr()
{
	return m_Filename;
}

//////////////////////////////////////////////////////////////////////////
void CVidDataSource::seek(unsigned long byte_index)
{
	if (m_File) m_File->Seek(byte_index);
}

//////////////////////////////////////////////////////////////////////////
unsigned long CVidDataSource::size()
{
	if (m_File) return m_File->GetSize();
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
unsigned long CVidDataSource::tell()
{
	if (m_File) return m_File->GetPos();
	else return 0;
}
