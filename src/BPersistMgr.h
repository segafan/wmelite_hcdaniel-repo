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

#ifndef __WmeBPersistMgr_H__
#define __WmeBPersistMgr_H__


#include "BBase.h"

class Vector2;

class CBPersistMgr : public CBBase
{
public:
	char* m_SavedDescription;
	time_t m_SavedTimestamp;
	BYTE m_SavedVerMajor;
	BYTE m_SavedVerMinor;
	BYTE m_SavedVerBuild;
	BYTE m_SavedExtMajor;
	BYTE m_SavedExtMinor;
	HRESULT SaveFile(char* Filename);
	DWORD GetDWORD();
	void PutDWORD(DWORD Val);
	char* GetString();
	void PutString(const char* Val);
	void Cleanup();
	HRESULT InitLoad(char* Filename);
	HRESULT InitSave(char* Desc);
	HRESULT GetBytes(BYTE* Buffer, DWORD Size);
	HRESULT PutBytes(BYTE* Buffer, DWORD Size);
	DWORD m_Offset;
	DWORD m_BufferSize;
	BYTE* m_Buffer;
	bool m_Saving;

	DWORD m_RichBufferSize;
	BYTE* m_RichBuffer;

	HRESULT Transfer(const char* Name, void* Val);
	HRESULT Transfer(const char* Name, int* Val);
	HRESULT Transfer(const char* Name, DWORD* Val);
	HRESULT Transfer(const char* Name, float* Val);
	HRESULT Transfer(const char* Name, double* Val);
	HRESULT Transfer(const char* Name, bool* Val);
	HRESULT Transfer(const char* Name, BYTE* Val);
	HRESULT Transfer(const char* Name, RECT* Val);
	HRESULT Transfer(const char* Name, POINT* Val);
	HRESULT Transfer(const char* Name, char** Val);
	HRESULT Transfer(const char* Name, Vector2* Val);
	HRESULT Transfer(const char* Name, AnsiStringArray& Val);
	CBPersistMgr(CBGame* inGame=NULL);
	virtual ~CBPersistMgr();
	bool CheckVersion(BYTE VerMajor, BYTE VerMinor, BYTE VerBuild);

	DWORD m_ThumbnailDataSize;
	BYTE* m_ThumbnailData;

};

#endif
