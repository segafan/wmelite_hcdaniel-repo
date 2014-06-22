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
#include "VidManager.h"
#include "VidTheoraPlayer.h"
#include <theoraplayer/TheoraPlayer.h>


//////////////////////////////////////////////////////////////////////////
CVidManager::CVidManager(CBGame* inGame):CBBase(inGame)
{
	m_Theora = NULL;
	m_PrevTime = 0;
}

//////////////////////////////////////////////////////////////////////////
CVidManager::~CVidManager()
{
	SAFE_DELETE(m_Theora);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidManager::InitLoop()
{
	DWORD currentTime = CBPlatform::GetTime();

	if (m_Theora && m_PrevTime > 0)
	{
		float timeIncrement = (float)(currentTime - m_PrevTime) / 1000.0f;
		m_Theora->update(timeIncrement);
	}
	m_PrevTime = currentTime;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidManager::Initialize()
{
	m_Theora = new TheoraVideoManager();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CVidManager::RegisterPlayer(CVidTheoraPlayer* player)
{
	m_Players.insert(player);
}

//////////////////////////////////////////////////////////////////////////
void CVidManager::UnregisterPlayer(CVidTheoraPlayer* player)
{
	m_Players.erase(player);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CVidManager::PauseAll()
{
	for (PlayerSet::iterator it = m_Players.begin(); it != m_Players.end(); ++it)
	{
		CVidTheoraPlayer* player = *it;
		if (player->IsPlaying())
		{
			player->Pause();
			player->SetFreezePaused(true);
		}
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CVidManager::ResumeAll()
{
	for (PlayerSet::iterator it = m_Players.begin(); it != m_Players.end(); ++it)
	{
		CVidTheoraPlayer* player = *it;
		if (player->IsFreezePaused())
		{
			player->Resume();
		}
	}
	return S_OK;
}
