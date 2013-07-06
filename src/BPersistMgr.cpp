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
#include "BPersistMgr.h"
#include "BSaveThumbHelper.h"
#include "Vector2.h"
#include "StringUtil.h"


#define SAVE_BUFFER_INIT_SIZE 100000
#define SAVE_BUFFER_GROW_BY   50000

#define SAVE_MAGIC		0x45564153
#define SAVE_MAGIC_2	0x32564153

//////////////////////////////////////////////////////////////////////////
CBPersistMgr::CBPersistMgr(CBGame* inGame):CBBase(inGame)
{
	m_Saving = false;
	m_Buffer = NULL;
	m_BufferSize = 0;
	m_Offset = 0;

	m_RichBuffer = NULL;
	m_RichBufferSize = 0;

	m_SavedDescription = NULL;
	m_SavedTimestamp = 0;
	m_SavedVerMajor = m_SavedVerMinor = m_SavedVerBuild = 0;
	m_SavedExtMajor = m_SavedExtMinor = 0;

	m_ThumbnailDataSize = 0;
	m_ThumbnailData = NULL;
}


//////////////////////////////////////////////////////////////////////////
CBPersistMgr::~CBPersistMgr()
{
	Cleanup();
}


//////////////////////////////////////////////////////////////////////////
void CBPersistMgr::Cleanup()
{
	if(m_Buffer)
	{
		if(m_Saving) free(m_Buffer);
		else delete [] m_Buffer; // allocated by file manager
	}
	m_Buffer = NULL;

	m_BufferSize = 0;
	m_Offset = 0;

	SAFE_DELETE_ARRAY(m_RichBuffer);
	m_RichBufferSize = 0;

	m_SavedDescription = NULL; // ref to buffer
	m_SavedTimestamp = 0;
	m_SavedVerMajor = m_SavedVerMinor = m_SavedVerBuild = 0;
	m_SavedExtMajor = m_SavedExtMinor = 0;

	m_ThumbnailDataSize = 0;
	if(m_ThumbnailData)
	{
		delete [] m_ThumbnailData;
		m_ThumbnailData = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBPersistMgr::InitSave(char* Desc)
{
	if(!Desc) return E_FAIL;

	HRESULT res;
	
	Cleanup();
	m_Saving = true;

	m_Buffer = (BYTE*)malloc(SAVE_BUFFER_INIT_SIZE);
	if(m_Buffer)
	{
		m_BufferSize = SAVE_BUFFER_INIT_SIZE;
		res = S_OK;
	}
	else res = E_FAIL;


	if(SUCCEEDED(res))
	{
		// get thumbnails
		if(!Game->m_CachedThumbnail)
		{
			Game->m_CachedThumbnail = new CBSaveThumbHelper(Game);
			if(FAILED(Game->m_CachedThumbnail->StoreThumbnail(true)))
			{
				SAFE_DELETE(Game->m_CachedThumbnail);
			}
		}


		DWORD magic = DCGF_MAGIC;
		PutDWORD(magic);

		magic = SAVE_MAGIC_2;
		PutDWORD(magic);

		BYTE VerMajor, VerMinor, ExtMajor, ExtMinor;
		Game->GetVersion(&VerMajor, &VerMinor, &ExtMajor, &ExtMinor);
		DWORD Version = MAKELONG(MAKEWORD(VerMajor, VerMinor), MAKEWORD(ExtMajor, ExtMinor));
		PutDWORD(Version);

		// new in ver 2
		PutDWORD((DWORD)DCGF_VER_BUILD);
		PutString(Game->m_Name);

		// thumbnail data size
		bool ThumbnailOK = false;

		if(Game->m_CachedThumbnail)
		{
			if(Game->m_CachedThumbnail->m_Thumbnail)
			{
				DWORD Size = 0;
				BYTE* Buffer = Game->m_CachedThumbnail->m_Thumbnail->CreateBMPBuffer(&Size);

				PutDWORD(Size);
				if(Size>0) PutBytes(Buffer, Size);

				delete [] Buffer;
				ThumbnailOK = true;
			}
		}
		if(!ThumbnailOK) PutDWORD(0);

		// in any case, destroy the cached thumbnail once used
		SAFE_DELETE(Game->m_CachedThumbnail);
			

		DWORD DataOffset = m_Offset + 
							sizeof(DWORD) + // data offset
							sizeof(DWORD) + strlen(Desc) + 1 + // description
							sizeof(DWORD); // timestamp

		PutDWORD(DataOffset);
		PutString(Desc);

		time_t Timestamp;
		time(&Timestamp);
		PutDWORD((DWORD)Timestamp);
	}
	return res;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBPersistMgr::InitLoad(char* Filename)
{
	Cleanup();

	m_Saving = false;

	m_Buffer = Game->m_FileManager->ReadWholeFile(Filename, &m_BufferSize);
	if(m_Buffer)
	{
		DWORD Magic;
		Magic = GetDWORD();
		if(Magic != DCGF_MAGIC) goto init_fail;

		Magic = GetDWORD();

		if(Magic==SAVE_MAGIC || Magic==SAVE_MAGIC_2)
		{
			DWORD Version = GetDWORD();
			m_SavedVerMajor = LOBYTE(LOWORD(Version));
			m_SavedVerMinor = HIBYTE(LOWORD(Version));
			m_SavedExtMajor = LOBYTE(HIWORD(Version));
			m_SavedExtMinor = HIBYTE(HIWORD(Version));

			if(Magic==SAVE_MAGIC_2)
			{
				m_SavedVerBuild = (BYTE)GetDWORD();
				char* SavedName = GetString();
				if(SavedName==NULL || CBPlatform::stricmp(SavedName, Game->m_Name)!=0)
				{
					Game->LOG(0, "ERROR: Saved game name doesn't match current game");
					goto init_fail;
				}

				// load thumbnail
				m_ThumbnailDataSize = GetDWORD();
				if(m_ThumbnailDataSize>0)
				{
					m_ThumbnailData = new BYTE[m_ThumbnailDataSize];
					if(m_ThumbnailData)
					{
						GetBytes(m_ThumbnailData, m_ThumbnailDataSize);
					}
					else m_ThumbnailDataSize = 0;
				}
			}
			else m_SavedVerBuild = 35; // last build with ver1 savegames


			// if save is newer version than we are, fail
			if( m_SavedVerMajor >  DCGF_VER_MAJOR ||
			   (m_SavedVerMajor == DCGF_VER_MAJOR && m_SavedVerMinor >  DCGF_VER_MINOR) ||
			   (m_SavedVerMajor == DCGF_VER_MAJOR && m_SavedVerMinor == DCGF_VER_MINOR && m_SavedVerBuild > DCGF_VER_BUILD)
			  )
			{
				Game->LOG(0, "ERROR: Saved game version is newer than current game");
				goto init_fail;				
			}
			
			// if save is older than the minimal version we support
			if( m_SavedVerMajor <  SAVEGAME_VER_MAJOR ||
				(m_SavedVerMajor == SAVEGAME_VER_MAJOR && m_SavedVerMinor <  SAVEGAME_VER_MINOR) ||
				(m_SavedVerMajor == SAVEGAME_VER_MAJOR && m_SavedVerMinor == SAVEGAME_VER_MINOR && m_SavedVerBuild < SAVEGAME_VER_BUILD)
				)
			{
				Game->LOG(0, "ERROR: Saved game is too old and cannot be used by this version of game engine");
				goto init_fail;				
			}

			/*
			if( m_SavedVerMajor != DCGF_VER_MAJOR || m_SavedVerMinor != DCGF_VER_MINOR)
			{
				Game->LOG(0, "ERROR: Saved game is created by other WME version");
				goto init_fail;				
			}
			*/
		}
		else goto init_fail;


		DWORD DataOffset = GetDWORD();

		m_SavedDescription = GetString();
		m_SavedTimestamp = (time_t)GetDWORD();

		m_Offset = DataOffset;

		return S_OK;
	}
	
init_fail:
	Cleanup();
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBPersistMgr::SaveFile(char *Filename)
{
	return Game->m_FileManager->SaveFile(Filename, m_Buffer, m_Offset, Game->m_CompressedSavegames, m_RichBuffer, m_RichBufferSize);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBPersistMgr::PutBytes(BYTE *Buffer, DWORD Size)
{
	while(m_Offset + Size > m_BufferSize)
	{
		m_BufferSize += SAVE_BUFFER_GROW_BY;
		m_Buffer = (BYTE*)realloc(m_Buffer, m_BufferSize);
		if(!m_Buffer)
		{
			Game->LOG(0, "Error reallocating save buffer to %d bytes", m_BufferSize);
			return E_FAIL;
		}
	}

	memcpy(m_Buffer+m_Offset, Buffer, Size);
	m_Offset+=Size;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBPersistMgr::GetBytes(BYTE *Buffer, DWORD Size)
{
	if(m_Offset+Size>m_BufferSize)
	{
		Game->LOG(0, "Fatal: Save buffer underflow");
		return E_FAIL;
	}

	memcpy(Buffer, m_Buffer+m_Offset, Size);
	m_Offset+=Size;
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CBPersistMgr::PutDWORD(DWORD Val)
{
	PutBytes((BYTE*)&Val, sizeof(DWORD));
}


//////////////////////////////////////////////////////////////////////////
DWORD CBPersistMgr::GetDWORD()
{
	DWORD ret;
	GetBytes((BYTE*)&ret, sizeof(DWORD));
	return ret;
}


//////////////////////////////////////////////////////////////////////////
void CBPersistMgr::PutString(const char* Val)
{
	if(!Val) PutString("(null)");
	else
	{
		PutDWORD(strlen(Val)+1);
		PutBytes((BYTE*)Val, strlen(Val)+1);
	}
}


//////////////////////////////////////////////////////////////////////////
char* CBPersistMgr::GetString()
{
	DWORD len = GetDWORD();
	char* ret = (char*)(m_Buffer+m_Offset);
	m_Offset+=len;

	if(!strcmp(ret, "(null)")) return NULL;
	else return ret;
}

//////////////////////////////////////////////////////////////////////////
// bool
HRESULT CBPersistMgr::Transfer(const char* Name, bool* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(bool));
	else return GetBytes((BYTE*)Val, sizeof(bool));
}


//////////////////////////////////////////////////////////////////////////
// int
HRESULT CBPersistMgr::Transfer(const char* Name, int* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(int));
	else return GetBytes((BYTE*)Val, sizeof(int));
}


//////////////////////////////////////////////////////////////////////////
// DWORD
HRESULT CBPersistMgr::Transfer(const char* Name, DWORD* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(DWORD));
	else return GetBytes((BYTE*)Val, sizeof(DWORD));
}


//////////////////////////////////////////////////////////////////////////
// float
HRESULT CBPersistMgr::Transfer(const char* Name, float* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(float));
	else return GetBytes((BYTE*)Val, sizeof(float));
}


//////////////////////////////////////////////////////////////////////////
// double
HRESULT CBPersistMgr::Transfer(const char* Name, double* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(double));
	else return GetBytes((BYTE*)Val, sizeof(double));
}


//////////////////////////////////////////////////////////////////////////
// char*
HRESULT CBPersistMgr::Transfer(const char* Name, char** Val)
{
	if(m_Saving)
	{
		PutString(*Val);
		return S_OK;
	}
	else
	{
		char* str = GetString();
		if(str)
		{

			*Val = new char[strlen(str)+1];
			strcpy(*Val, str);
		}
		else *Val = NULL;
		return S_OK;
	}
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBPersistMgr::Transfer(const char* Name, AnsiStringArray& Val)
{
	size_t size;

	if (m_Saving)
	{
		size = Val.size();
		PutBytes((BYTE*)&size, sizeof(size_t));

		for (AnsiStringArray::iterator it = Val.begin(); it != Val.end(); ++it)
		{
			PutString((*it).c_str());
		}
	}
	else
	{
		Val.clear();
		GetBytes((BYTE*)&size, sizeof(size_t));

		for (size_t i = 0; i < size; i++)
		{
			char* str = GetString();
			if (str) Val.push_back(str);
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// BYTE
HRESULT CBPersistMgr::Transfer(const char* Name, BYTE* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(BYTE));
	else return GetBytes((BYTE*)Val, sizeof(BYTE));
}


//////////////////////////////////////////////////////////////////////////
// RECT
HRESULT CBPersistMgr::Transfer(const char* Name, RECT* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(RECT));
	else return GetBytes((BYTE*)Val, sizeof(RECT));
}


//////////////////////////////////////////////////////////////////////////
// POINT
HRESULT CBPersistMgr::Transfer(const char* Name, POINT* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(POINT));
	else return GetBytes((BYTE*)Val, sizeof(POINT));
}


//////////////////////////////////////////////////////////////////////////
// Vector2
HRESULT CBPersistMgr::Transfer(const char* Name, Vector2* Val)
{
	if(m_Saving) return PutBytes((BYTE*)Val, sizeof(Vector2));
	else return GetBytes((BYTE*)Val, sizeof(Vector2));
}


//////////////////////////////////////////////////////////////////////////
// generic pointer
HRESULT CBPersistMgr::Transfer(const char* Name, void* Val)
{
	int ClassID=-1, InstanceID=-1;

	if(m_Saving)
	{
		CSysClassRegistry::GetInstance()->GetPointerID(*(void**)Val, &ClassID, &InstanceID);
		if(*(void**)Val!= NULL && (ClassID==-1 || InstanceID==-1))
		{
			Game->LOG(0, "Warning: invalid instance '%s'", Name);
		}

		PutDWORD(ClassID);
		PutDWORD(InstanceID);
	}
	else
	{
		ClassID = GetDWORD();
		InstanceID = GetDWORD();

		*(void**)Val = CSysClassRegistry::GetInstance()->IDToPointer(ClassID, InstanceID);
	}
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
bool CBPersistMgr::CheckVersion(BYTE VerMajor, BYTE VerMinor, BYTE VerBuild)
{
	if(m_Saving) return true;

	// it's ok if we are same or newer than the saved game
	if( VerMajor >  m_SavedVerMajor ||
	   (VerMajor == m_SavedVerMajor && VerMinor >  m_SavedVerMinor) ||
	   (VerMajor == m_SavedVerMajor && VerMinor == m_SavedVerMinor && VerBuild > m_SavedVerBuild)
	  ) return false;

	return true;
}
