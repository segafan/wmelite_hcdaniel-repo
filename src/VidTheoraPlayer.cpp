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
#include "VidTheoraPlayer.h"
#include "VidDataSource.h"
#include "BSurfaceSDL.h"
#include <theoraplayer/TheoraPlayer.h>


IMPLEMENT_PERSISTENT(CVidTheoraPlayer, false);

//////////////////////////////////////////////////////////////////////////
CVidTheoraPlayer::CVidTheoraPlayer(CBGame* inGame):CBBase(inGame)
{
	SetDefaults();
}

//////////////////////////////////////////////////////////////////////////
CVidTheoraPlayer::~CVidTheoraPlayer()
{
	Cleanup();

	SAFE_DELETE_ARRAY(m_Filename);
	SAFE_DELETE_ARRAY(m_AlphaFilename);
	SAFE_DELETE(m_Texture);
	SAFE_DELETE(m_AlphaTexture);
}

//////////////////////////////////////////////////////////////////////////
void CVidTheoraPlayer::SetDefaults()
{
	m_Clip = NULL;
	m_File = NULL;
	m_Filename = NULL;
	m_Looping = false;
	//m_Sound = NULL;
	m_FreezeGame = false;
	m_FreezePaused = false;
	m_CurrentTime = 0;

	m_State = THEORA_STATE_NONE;
	m_Texture = NULL;
	m_AlphaTexture = NULL;
	m_AlphaFilename = NULL;

	m_SavedState = THEORA_STATE_NONE;
	m_SavedPos = 0;
}

//////////////////////////////////////////////////////////////////////////
void CVidTheoraPlayer::Cleanup()
{
	if (m_Clip)
	{
		Game->m_VideoMgr->GetTheora()->destroyVideoClip(m_Clip);
		m_Clip = NULL;
	}

	if (m_File) Game->m_FileManager->CloseFile(m_File);
	m_File = NULL;

	/*
	if (m_Sound)
	{
		Game->m_SoundMgr->RemoveSound(m_Sound);
		m_Sound = NULL;
	}
	*/

	Game->m_VideoMgr->UnregisterPlayer(this);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::Initialize(char* filename, char* subtitleFile)
{
	Cleanup();

	m_File = Game->m_FileManager->OpenFile(filename);
	if (!m_File) return E_FAIL;

	if (filename != m_Filename) CBUtils::SetString(&m_Filename, filename);

	m_Clip = Game->m_VideoMgr->GetTheora()->createVideoClip(new CVidDataSource(Game, m_File, m_Filename), TH_XBGR);

	if (m_Clip)
	{
		m_Texture = new CBSurfaceSDL(Game);
		m_Texture->Create(m_Clip->getWidth(), m_Clip->getHeight());
	}

	Game->m_VideoMgr->RegisterPlayer(this);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::InitializeSimple()
{
	if (SUCCEEDED(Initialize(m_Filename)))
	{
		if (m_AlphaFilename) SetAlphaImage(m_AlphaFilename);
		Play(m_PlaybackType, m_PosX, m_PosY, false, false, m_Looping, m_SavedPos, m_PlayZoom);
	}
	else m_State = THEORA_STATE_FINISHED;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::Update()
{
	if (!m_Clip) return E_FAIL;

	if (m_Clip->isDone())
	{
		Stop();
		return S_OK;
	}

	TheoraVideoFrame* frame = m_Clip->getNextFrame();
	if (frame)
	{
		// transfer the frame pixels to your display device, texure, graphical context or whatever you use.
		m_Texture->FillTexture(frame->getBuffer(), frame->getStride() * 4, m_AlphaTexture);
		m_Clip->popFrame();
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::Play(TVideoPlayback type, int x, int y, bool freezeGame, bool freezeMusic, bool looping, DWORD startTime, float forceZoom, int volume)
{
	if (forceZoom < 0.0f) forceZoom = 100.0f;
	if (volume < 0) m_Volume = Game->m_SoundMgr->GetVolumePercent(SOUND_SFX);
	else m_Volume = volume;

	m_FreezeGame = freezeGame;
	if (m_FreezeGame) Game->Freeze(freezeMusic);

	m_State = THEORA_STATE_PLAYING;

	m_Looping = looping;
	m_PlaybackType = type;

	float width = 0, height = 0;
	if (m_Clip)
	{
		width = m_Clip->getWidth();
		height = m_Clip->getHeight();
		m_Clip->setAutoRestart(looping);
	}
	else
	{
		width = (float)Game->m_Renderer->m_Width;
		height = (float)Game->m_Renderer->m_Height;
	}

	switch (type)
	{
	case VID_PLAY_POS:
		m_PlayZoom = forceZoom;
		m_PosX = x;
		m_PosY = y;
		break;

	case VID_PLAY_STRETCH:
		{
			float ZoomX = (float)((float)Game->m_Renderer->m_Width / width * 100);
			float ZoomY = (float)((float)Game->m_Renderer->m_Height / height * 100);
			m_PlayZoom = min(ZoomX, ZoomY);
			m_PosX = (Game->m_Renderer->m_Width - width * (m_PlayZoom / 100)) / 2;
			m_PosY = (Game->m_Renderer->m_Height - height * (m_PlayZoom / 100)) / 2;
		}
		break;

	case VID_PLAY_CENTER:
		m_PlayZoom = 100.0f;
		m_PosX = (Game->m_Renderer->m_Width - width) / 2;
		m_PosY = (Game->m_Renderer->m_Height - height) / 2;
		break;
	}


	if (startTime) SeekToTime(startTime);

	Update();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::Stop()
{
	//if (m_Sound) m_Sound->Stop();
	m_State = THEORA_STATE_FINISHED;
	if (m_Clip) m_Clip->stop();
	if (m_FreezeGame) Game->Unfreeze();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::Display(DWORD alpha)
{
	RECT rc;
	HRESULT Res;

	if (m_Texture)
	{
		SetRect(&rc, 0, 0, m_Texture->GetWidth(), m_Texture->GetHeight());
		if (m_PlayZoom == 100.0f) Res = m_Texture->DisplayTrans(m_PosX, m_PosY, rc, alpha);
		else Res = m_Texture->DisplayTransZoom(m_PosX, m_PosY, rc, m_PlayZoom, m_PlayZoom, alpha);
	}
	else Res = E_FAIL;

	return Res;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::Pause()
{
	if(m_State == THEORA_STATE_PLAYING)
	{
		m_State = THEORA_STATE_PAUSED;
		if (m_Clip) m_Clip->pause();
		//if (m_Sound) m_Sound->Pause();
		return S_OK;
	}
	else return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::Resume()
{
	if (m_State == THEORA_STATE_PAUSED)
	{
		m_State = THEORA_STATE_PLAYING;
		if (m_Clip) m_Clip->play();
		//if (m_Sound) m_Sound->Resume();
		m_FreezePaused = false;
		return S_OK;
	}
	else return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::SetAlphaImage(char* filename)
{
	SAFE_DELETE(m_AlphaTexture);

	m_AlphaTexture = new CBSurfaceSDL(Game);
	if (FAILED(m_AlphaTexture->Create(filename, false, 0, 0, 0)))
	{
		SAFE_DELETE(m_AlphaTexture);
		SAFE_DELETE_ARRAY(m_AlphaFilename);
		return E_FAIL;
	}
	if (m_AlphaFilename != filename) CBUtils::SetString(&m_AlphaFilename, filename);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::SeekToTime(DWORD time)
{
	if (m_Clip)
	{
		m_Clip->seek((float)time / 1000.0f);
		return S_OK;
	}
	else return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CVidTheoraPlayer::Persist(CBPersistMgr* persistMgr)
{
	if (persistMgr->m_Saving)
	{
		m_SavedPos = GetMovieTime() * 1000;
		m_SavedState = m_State;
	}
	else
	{
		SetDefaults();
	}

	persistMgr->Transfer(TMEMBER(Game));
	persistMgr->Transfer(TMEMBER(m_SavedPos));
	persistMgr->Transfer(TMEMBER(m_SavedState));
	persistMgr->Transfer(TMEMBER(m_Filename));
	persistMgr->Transfer(TMEMBER(m_AlphaFilename));
	persistMgr->Transfer(TMEMBER(m_PosX));
	persistMgr->Transfer(TMEMBER(m_PosY));
	persistMgr->Transfer(TMEMBER(m_PlayZoom));
	persistMgr->Transfer(TMEMBER_INT(m_PlaybackType));
	persistMgr->Transfer(TMEMBER(m_Looping));
	persistMgr->Transfer(TMEMBER(m_Volume));
	persistMgr->Transfer(TMEMBER(m_FreezePaused));

	return S_OK;

}

//////////////////////////////////////////////////////////////////////////
float CVidTheoraPlayer::GetMovieTime()
{
	if (m_Clip) return m_Clip->getTimePosition();
	else return 0.0f;
}