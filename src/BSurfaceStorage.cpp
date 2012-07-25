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
#include "BSurfaceStorage.h"
#include "BSurfaceSDL.h"


//IMPLEMENT_PERSISTENT(CBSurfaceStorage, true);

//////////////////////////////////////////////////////////////////////
CBSurfaceStorage::CBSurfaceStorage(CBGame* inGame):CBBase(inGame)
{
	m_LastCleanupTime = 0;
}


//////////////////////////////////////////////////////////////////////
CBSurfaceStorage::~CBSurfaceStorage()
{
	Cleanup(true);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceStorage::Cleanup(bool Warn)
{
	for(int i=0; i<m_Surfaces.GetSize(); i++)
	{
		if(Warn) Game->LOG(0, "CBSurfaceStorage warning: purging surface '%s', usage:%d", m_Surfaces[i]->m_Filename, m_Surfaces[i]->m_ReferenceCount);
		delete m_Surfaces[i];
	}
	m_Surfaces.RemoveAll();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceStorage::InitLoop()
{
	if(Game->m_SmartCache && Game->m_LiveTimer - m_LastCleanupTime >= Game->m_SurfaceGCCycleTime)
	{
		m_LastCleanupTime = Game->m_LiveTimer;
		SortSurfaces();
		for(int i=0; i<m_Surfaces.GetSize(); i++)
		{
			if(m_Surfaces[i]->m_LifeTime <= 0) break;

			if(m_Surfaces[i]->m_LifeTime > 0 && m_Surfaces[i]->m_Valid && Game->m_LiveTimer - m_Surfaces[i]->m_LastUsedTime >= m_Surfaces[i]->m_LifeTime)
			{
				//Game->QuickMessageForm("Invalidating: %s", m_Surfaces[i]->m_Filename);
				m_Surfaces[i]->Invalidate();
			}
		}
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceStorage::RemoveSurface(CBSurface * surface)
{
	for(int i=0; i<m_Surfaces.GetSize(); i++)
	{
		if(m_Surfaces[i]==surface)
		{
			m_Surfaces[i]->m_ReferenceCount--;
			if(m_Surfaces[i]->m_ReferenceCount <= 0)
			{
				delete m_Surfaces[i];
				m_Surfaces.RemoveAt(i);
			}
			break;
		}
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
CBSurface* CBSurfaceStorage::AddSurface(char * Filename, bool default_ck, BYTE ck_red, BYTE ck_green, BYTE ck_blue, int LifeTime, bool KeepLoaded)
{
	for(int i=0; i<m_Surfaces.GetSize(); i++)
	{
		if(CBPlatform::stricmp(m_Surfaces[i]->m_Filename, Filename)==0)
		{
			m_Surfaces[i]->m_ReferenceCount++;
			return m_Surfaces[i];
		}
	}

	CBFile* File = Game->m_FileManager->OpenFile(Filename);
	if(!File)
	{
		if(Filename) Game->LOG(0, "Missing image: '%s'", Filename);
		if(Game->m_DEBUG_DebugMode)
			return AddSurface("invalid_debug.bmp", default_ck, ck_red, ck_green, ck_blue, LifeTime, KeepLoaded);
		else
			return AddSurface("invalid.bmp", default_ck, ck_red, ck_green, ck_blue, LifeTime, KeepLoaded);
	}
	else Game->m_FileManager->CloseFile(File);


	CBSurface* surface;
	surface = new CBSurfaceSDL(Game);


	if(!surface) return NULL;

	if(FAILED(surface->Create(Filename, default_ck, ck_red, ck_green, ck_blue, LifeTime, KeepLoaded)))
	{
		delete surface;
		return NULL;
	}
	else
	{
		surface->m_ReferenceCount = 1;
		m_Surfaces.Add(surface);
		return surface;
	}
}


//////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceStorage::RestoreAll()
{
	HRESULT ret;
	for(int i=0; i<m_Surfaces.GetSize(); i++)
	{
		ret = m_Surfaces[i]->Restore();
		if(ret!=S_OK)
		{
			Game->LOG(0, "CBSurfaceStorage::RestoreAll failed");
			return ret;
		}
	}
	return S_OK;
}


/*
//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceStorage::Persist(CBPersistMgr *PersistMgr)
{

	if(!PersistMgr->m_Saving) Cleanup(false);

	PersistMgr->Transfer(TMEMBER(Game));

	//m_Surfaces.Persist(PersistMgr);

	return S_OK;
}
*/


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceStorage::SortSurfaces()
{
	qsort(m_Surfaces.GetData(), m_Surfaces.GetSize(), sizeof(CBSurface*), SurfaceSortCB);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
int CBSurfaceStorage::SurfaceSortCB(const void* arg1, const void* arg2)
{
	CBSurface* s1 = *((CBSurface**)arg1);
	CBSurface* s2 = *((CBSurface**)arg2);

	// sort by life time
	if(s1->m_LifeTime <= 0 && s2->m_LifeTime > 0) return 1;
	else if(s1->m_LifeTime > 0 && s2->m_LifeTime <= 0) return -1;


	// sort by validity
	if(s1->m_Valid && !s2->m_Valid) return -1;
	else if(!s1->m_Valid && s2->m_Valid) return 1;

	// sort by time
	else if(s1->m_LastUsedTime > s2->m_LastUsedTime) return 1;
	else if(s1->m_LastUsedTime < s2->m_LastUsedTime) return -1;
	else return 0;
}
