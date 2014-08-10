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
#include "BSound.h"


IMPLEMENT_PERSISTENT(CBSound, false);

//////////////////////////////////////////////////////////////////////////
CBSound::CBSound(CBGame* inGame):CBBase(inGame)
{
	m_Sound = NULL;
	m_SoundFilename = NULL;

	m_SoundType = SOUND_SFX;
	m_SoundStreamed = false;
	m_SoundLooping = false;
	m_SoundPlaying = false;
	m_SoundPaused = false;
	m_SoundFreezePaused = false;
	m_SoundPosition = 0;
	m_SoundPrivateVolume = 0;
	m_SoundLoopStart = 0;

	m_SFXType = SFX_NONE;
	m_SFXParam1 = m_SFXParam2 = m_SFXParam3 = m_SFXParam4 = 0;
}


//////////////////////////////////////////////////////////////////////////
CBSound::~CBSound()
{
	if(m_Sound) Game->m_SoundMgr->RemoveSound(m_Sound);
	m_Sound = NULL;

	SAFE_DELETE_ARRAY(m_SoundFilename);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::SetSound(char* Filename, TSoundType Type, bool Streamed, DWORD initialPrivateVolume)
{
	if(m_Sound){
		Game->m_SoundMgr->RemoveSound(m_Sound);
		m_Sound = NULL;
	}
	SAFE_DELETE_ARRAY(m_SoundFilename);

	m_Sound = Game->m_SoundMgr->AddSound(Filename, Type, Streamed, initialPrivateVolume);
	if(m_Sound){
		m_SoundFilename = new char[strlen(Filename)+1];
		strcpy(m_SoundFilename, Filename);

		m_SoundType = Type;
		m_SoundStreamed = Streamed;

		return S_OK;
	}
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::SetSoundSimple()
{
	m_Sound = Game->m_SoundMgr->AddSound(m_SoundFilename, m_SoundType, m_SoundStreamed);
	if(m_Sound){
		if(m_SoundPosition) m_Sound->SetPosition(m_SoundPosition);
		m_Sound->SetLooping(m_SoundLooping);
		m_Sound->SetPrivateVolume(m_SoundPrivateVolume);
		m_Sound->SetLoopStart(m_SoundLoopStart);
		m_Sound->m_FreezePaused = m_SoundFreezePaused;
		if(m_SoundPlaying) return m_Sound->Resume();
		else return S_OK;
	}
	else return E_FAIL;
}



//////////////////////////////////////////////////////////////////////////
DWORD CBSound::GetLength()
{
	if(m_Sound) return m_Sound->GetLength();
	else return 0;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::Play(bool Looping)
{	
	if(m_Sound){
		m_SoundPaused = false;		
		return m_Sound->Play(Looping, m_SoundPosition);
	}
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::Stop()
{
	if(m_Sound){
		m_SoundPaused = false;
		return m_Sound->Stop();
	}
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::Pause(bool FreezePaused)
{
	if(m_Sound){
		m_SoundPaused = true;
		if(FreezePaused) m_Sound->m_FreezePaused = true;
		return m_Sound->Pause();
	}
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::Resume()
{
	if(m_Sound && m_SoundPaused){
		m_SoundPaused = false;
		return m_Sound->Resume();
	}
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::Persist(CBPersistMgr *PersistMgr)
{
	if(PersistMgr->m_Saving && m_Sound){
		m_SoundPlaying = m_Sound->IsPlaying();
		m_SoundLooping = m_Sound->m_Looping;
		m_SoundPrivateVolume = m_Sound->m_PrivateVolume;
		if(m_SoundPlaying) m_SoundPosition = m_Sound->GetPosition();
		m_SoundLoopStart = m_Sound->m_LoopStart;
		m_SoundFreezePaused = m_Sound->m_FreezePaused;
	}

	if(PersistMgr->m_Saving)
	{
		m_SFXType = SFX_NONE;
		m_SFXParam1 = m_SFXParam2 = m_SFXParam3 = m_SFXParam4 = 0;
	}

	PersistMgr->Transfer(TMEMBER(Game));

	PersistMgr->Transfer(TMEMBER(m_SoundFilename));
	PersistMgr->Transfer(TMEMBER(m_SoundLooping));
	PersistMgr->Transfer(TMEMBER(m_SoundPaused));
	PersistMgr->Transfer(TMEMBER(m_SoundFreezePaused));
	PersistMgr->Transfer(TMEMBER(m_SoundPlaying));
	PersistMgr->Transfer(TMEMBER(m_SoundPosition));
	PersistMgr->Transfer(TMEMBER(m_SoundPrivateVolume));
	PersistMgr->Transfer(TMEMBER(m_SoundStreamed));
	PersistMgr->Transfer(TMEMBER_INT(m_SoundType));
	PersistMgr->Transfer(TMEMBER(m_SoundLoopStart));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
bool CBSound::IsPlaying()
{
	return m_Sound && m_Sound->IsPlaying();
}


//////////////////////////////////////////////////////////////////////////
bool CBSound::IsPaused()
{
	return m_Sound && m_SoundPaused;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::SetPositionTime(DWORD Time)
{
	if(!m_Sound) return E_FAIL;
	m_SoundPosition = Time;
	HRESULT ret = m_Sound->SetPosition(m_SoundPosition);
	if(m_Sound->IsPlaying()) m_SoundPosition = 0;
	return ret;
}


//////////////////////////////////////////////////////////////////////////
DWORD CBSound::GetPositionTime()
{
	if(!m_Sound) return 0;

	if(!m_Sound->IsPlaying()) return 0;
	else return m_Sound->GetPosition();
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::SetVolume(int Volume)
{
	if(!m_Sound) return E_FAIL;
	else return m_Sound->SetPrivateVolume(Volume);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::SetPrivateVolume(int Volume)
{
	if(!m_Sound) return E_FAIL;
	else return m_Sound->m_PrivateVolume = Volume;
}


//////////////////////////////////////////////////////////////////////////
int CBSound::GetVolume()
{
	if(!m_Sound) return 0;
	else return m_Sound->m_PrivateVolume;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::SetLoopStart(DWORD Pos)
{
	if(!m_Sound) return E_FAIL;
	else
	{
		m_Sound->SetLoopStart(Pos);
		return S_OK;
	}
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::SetPan(float Pan)
{
	if(m_Sound) return m_Sound->SetPan(Pan);
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSound::ApplyFX(TSFXType Type, float Param1, float Param2, float Param3, float Param4)
{
	if(!m_Sound) return S_OK;

	if(Type!=m_SFXType || Param1!=m_SFXParam1 || Param2!=m_SFXParam2 || Param3!=m_SFXParam3 || Param4!=m_SFXParam4)
	{
		HRESULT Ret = m_Sound->ApplyFX(Type, Param1, Param2, Param3, Param4);

		m_SFXType = Type;
		m_SFXParam1 = Param1;
		m_SFXParam2 = Param2;
		m_SFXParam3 = Param3;
		m_SFXParam4 = Param4;

		return Ret;
	}
	return S_OK;
}
