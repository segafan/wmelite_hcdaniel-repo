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
#include "BDiskFile.h"
#include "PathUtil.h"

//////////////////////////////////////////////////////////////////////////
CBDiskFile::CBDiskFile(CBGame* inGame):CBFile(inGame)
{
	m_File = NULL;
	m_Data = NULL;
	m_Compressed = false;
	m_PrefixSize = 0;
}


//////////////////////////////////////////////////////////////////////////
CBDiskFile::~CBDiskFile()
{
	Close();	
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDiskFile::Open(const char* Filename)
{
	Close();

	char FullPath[MAX_PATH];

	for(int i=0; i<Game->m_FileManager->m_SinglePaths.GetSize(); i++)
	{
		sprintf(FullPath, "%s%s", Game->m_FileManager->m_SinglePaths[i], Filename);
		CorrectSlashes(FullPath);

		// re-check the access method for every path
		ops = PathUtil::GetFileAccessMethod(FullPath);

		m_File = ops->file_open(FullPath, "rb");
		if(m_File!=NULL) break;
	}

	// if we didn't find it in search paths, try to open directly
	if(!m_File)
	{
		strcpy(FullPath, Filename);
		CorrectSlashes(FullPath);
		ops = PathUtil::GetFileAccessMethod(FullPath);
		m_File = ops->file_open(FullPath, "rb");
	}

	if(m_File)
	{
		DWORD magic1, magic2;
		ops->file_read((char *) (&magic1), sizeof(DWORD), m_File);
		ops->file_read((char *) (&magic2), sizeof(DWORD), m_File);


		if(magic1==DCGF_MAGIC && magic2==COMPRESSED_FILE_MAGIC) m_Compressed = true;

		if(m_Compressed)
		{
			DWORD DataOffset, CompSize, UncompSize;
			ops->file_read((char *) (&DataOffset), sizeof(DWORD), m_File);
			ops->file_read((char *) (&CompSize), sizeof(DWORD), m_File);
			ops->file_read((char *) (&UncompSize), sizeof(DWORD), m_File);

			BYTE* CompBuffer = new BYTE[CompSize];
			if(!CompBuffer)
			{
				Game->LOG(0, "Error allocating memory for compressed file '%s'", Filename);
				Close();
				return E_FAIL;
			}

			m_Data = new BYTE[UncompSize];
			if(!m_Data)
			{
				Game->LOG(0, "Error allocating buffer for file '%s'", Filename);
				delete [] CompBuffer;
				Close();
				return E_FAIL;
			}

			ops->file_seek(m_File, DataOffset + m_PrefixSize, SEEK_SET);
			ops->file_read((char *) CompBuffer, CompSize, m_File);

			if(uncompress(m_Data, (uLongf*)&UncompSize, CompBuffer, CompSize)!=Z_OK)
			{
				Game->LOG(0, "Error uncompressing file '%s'", Filename);
				delete [] CompBuffer;
				Close();
				return E_FAIL;
			}

			delete [] CompBuffer;
			m_Size = UncompSize;
			m_Pos = 0;
			ops->file_close(m_File);
			m_File = NULL;
		}
		else
		{
			m_Pos = 0;
			ops->file_seek(m_File, 0, SEEK_END);
			m_Size = ops->file_tell(m_File) - m_PrefixSize;
			ops->file_seek(m_File, m_PrefixSize, SEEK_SET);
		}

		return S_OK;
	}
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDiskFile::Close()
{
	if(m_File) ops->file_close(m_File);
	m_File = NULL;
	m_Pos = 0;
	m_Size = 0;

	SAFE_DELETE_ARRAY(m_Data);
	m_Compressed = false;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDiskFile::Read(void *Buffer, DWORD Size)
{
	if(m_Compressed)
	{
		memcpy(Buffer, m_Data+m_Pos, Size);
		m_Pos+=Size;
		return S_OK;
	}
	else
	{
		if(m_File)			
		{
			size_t count = ops->file_read((char *) Buffer, Size, m_File);
			if (count == 0) 
				return E_FAIL;
			m_Pos += count;
			return S_OK;
		}
		else return E_FAIL;
	}
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBDiskFile::Seek(DWORD Pos, TSeek Origin)
{
	if(m_Compressed)
	{
		DWORD NewPos=0;

		switch(Origin)
		{
			case SEEK_TO_BEGIN:   NewPos = Pos;          break;
			case SEEK_TO_END:     NewPos = m_Size + Pos; break;
			case SEEK_TO_CURRENT: NewPos = m_Pos + Pos;  break;
		}

		if(NewPos > m_Size) return E_FAIL;
		else m_Pos = NewPos;
		return S_OK;
	}
	else
	{
		if(!m_File) return E_FAIL;

		int ret=1;

		switch(Origin)
		{
			case SEEK_TO_BEGIN:   ret = ops->file_seek(m_File, m_PrefixSize + Pos, SEEK_SET); break;
			case SEEK_TO_END:     ret = ops->file_seek(m_File, Pos, SEEK_END); break;
			case SEEK_TO_CURRENT: ret = ops->file_seek(m_File, Pos, SEEK_CUR); break;
		}
		if(ret==0)
		{
			m_Pos = ops->file_tell(m_File) - m_PrefixSize;
			return S_OK;
		}
		else return E_FAIL;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBDiskFile::CorrectSlashes(char* fileName)
{
	for (size_t i = 0; i < strlen(fileName); i++)
	{
		if (fileName[i] == '\\') fileName[i] = '/';
	}
}
