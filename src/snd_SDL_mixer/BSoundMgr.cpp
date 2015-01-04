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
#include "BSoundMgr.h"
#include "PathUtil.h"
#include "StringUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_PERSISTENT(CBSoundMgr, true);

//////////////////////////////////////////////////////////////////////////
CBSoundMgr::CBSoundMgr(CBGame* inGame):CBBase(inGame)
{
	m_SoundAvailable = false;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		Game->LOG(0, "SDL_InitSubSystem failed to init audio!");
	}

	m_VolumeSFX = m_VolumeSpeech = m_VolumeMusic = m_VolumeMaster = 100;
}


//////////////////////////////////////////////////////////////////////////
CBSoundMgr::~CBSoundMgr()
{
	SaveSettings();
	Cleanup();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::Cleanup()
{
	for(int i=0; i<m_Sounds.GetSize(); i++) delete m_Sounds[i];
	m_Sounds.RemoveAll();

	Mix_CloseAudio();
	Mix_Quit();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CBSoundMgr::SaveSettings()
{
	if (m_SoundAvailable)
	{
		Game->m_Registry->WriteInt("Audio", "MasterVolume", m_VolumeMaster);
		
		Game->m_Registry->WriteInt("Audio", "SFXVolume",    m_VolumeSFX);
		Game->m_Registry->WriteInt("Audio", "SpeechVolume", m_VolumeSpeech);
		Game->m_Registry->WriteInt("Audio", "MusicVolume",  m_VolumeMusic);
	}	
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::Initialize()
{
	m_SoundAvailable = false;


#ifdef __IPHONEOS__
	#define BASS_CONFIG_IOS_MIXAUDIO 34
    BASS_SetConfig(BASS_CONFIG_IOS_MIXAUDIO, 6);
#endif

	// check proper version of SDL_mixer
	SDL_version compile_version;
	const SDL_version *link_version = Mix_Linked_Version();
	SDL_MIXER_VERSION(&compile_version);
	
	// major and minor shall match, ignore patch version
	if ((compile_version.major != link_version->major) || (compile_version.minor != link_version->minor)) {
		Game->LOG(0, "compiled with SDL_mixer version: %d.%d.%d\n", 
		          compile_version.major,
        		  compile_version.minor,
        		  compile_version.patch);
		Game->LOG(0, "running with SDL_mixer version: %d.%d.%d\n", 
        		  link_version->major,
        		  link_version->minor,
        		  link_version->patch);
		Game->LOG(0, "SDL_mixer major/minor version mismatch!");

		return E_FAIL;
	}

	if ((Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG) != MIX_INIT_OGG) {
		Game->LOG(0, "Can't initialize SDL_mixer");
		return E_FAIL;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
		Game->LOG(0, "Can't initialize sound device");
		return E_FAIL;
	}
	Mix_ChannelFinished((void *) this, SDLMixer_channelDoneCallback);
	Mix_HookMusicFinishedCh((void *) this, SDLMixer_musicFinishedHook);

	m_VolumeMaster = Game->m_Registry->ReadInt("Audio", "MasterVolume", 100);

	m_VolumeSFX    = Game->m_Registry->ReadInt("Audio", "SFXVolume",    100);
	m_VolumeSpeech = Game->m_Registry->ReadInt("Audio", "SpeechVolume", 100);
	m_VolumeMusic  = Game->m_Registry->ReadInt("Audio", "MusicVolume",  100);


	m_SoundAvailable = true;
	SetMasterVolumePercent(m_VolumeMaster);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::InitLoop()
{
	if(!m_SoundAvailable) return S_OK;

	// TODO does SDL_mixer need a frequent call?
	// BASS_Update(500);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
CBSoundBuffer* CBSoundMgr::AddSound(const char *Filename, TSoundType Type, bool Streamed, DWORD initialPrivateVolume)
{
	if(!m_SoundAvailable) return NULL;

	CBSoundBuffer* sound;
	
	// try to switch WAV to OGG file (if available)
	AnsiString ext = PathUtil::GetExtension(Filename);
	if (StringUtil::CompareNoCase(ext, ".wav"))
	{
		AnsiString path = PathUtil::GetDirectoryName(Filename);
		AnsiString name = PathUtil::GetFileNameWithoutExtension(Filename);

		AnsiString newFile = PathUtil::Combine(path, name + ".ogg");
		CBFile* file = Game->m_FileManager->OpenFile(newFile.c_str());
		if(file)
		{
			Filename = newFile.c_str();
			Game->m_FileManager->CloseFile(file);
		}
	}

	sound = new CBSoundBuffer(Game);
	if(!sound) return NULL;

	sound->SetStreaming(Streamed);
	sound->SetType(Type);


	HRESULT res = sound->LoadFromFile(Filename);
	if(FAILED(res))
	{
		Game->LOG(res, "Error loading sound '%s'", Filename);
		delete sound;
		return NULL;
	}

	// sound starts with user defined instead of 100% volume (of the global setting)
	sound->m_PrivateVolume = initialPrivateVolume;

	// set volume appropriately (global volume setting)
	switch(Type)
	{
	case SOUND_SFX:
		sound->SetVolume(m_VolumeSFX);
		break;
	case SOUND_SPEECH:
		sound->SetVolume(m_VolumeSpeech);
		break;
	case SOUND_MUSIC:
		sound->SetVolume(m_VolumeMusic);
		break;
	}

	// register sound
	m_Sounds.Add(sound);

	return sound;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::AddSound(CBSoundBuffer* Sound, TSoundType Type)
{
	if(!Sound) return E_FAIL;

	// set volume appropriately
	switch(Type)
	{
		case SOUND_SFX:    Sound->SetVolume(m_VolumeSFX);    break;
		case SOUND_SPEECH: Sound->SetVolume(m_VolumeSpeech); break;
		case SOUND_MUSIC:  Sound->SetVolume(m_VolumeMusic);  break;
	}

	// register sound
	m_Sounds.Add(Sound);
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::RemoveSound(CBSoundBuffer* Sound)
{
	for (int i=0; i<m_Sounds.GetSize(); i++)
	{
		if (m_Sounds[i]==Sound)
		{
			delete m_Sounds[i];
			m_Sounds.RemoveAt(i);
			return S_OK;
		}
	}
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::SetVolume(TSoundType Type, int Volume)
{
	if (!m_SoundAvailable) return S_OK;

	switch (Type)
	{
	case SOUND_SFX:
		m_VolumeSFX = Volume;
		break;
	case SOUND_SPEECH:
		m_VolumeSpeech = Volume;
		break;
	case SOUND_MUSIC:
		m_VolumeMusic  = Volume;
		break;
	}

	for (int i=0; i<m_Sounds.GetSize(); i++)
	{
		if (m_Sounds[i]->m_Type==Type) m_Sounds[i]->SetVolume(Volume);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::SetVolumePercent(TSoundType Type, BYTE Percent)
{
	return SetVolume(Type, Percent);
}


//////////////////////////////////////////////////////////////////////////
BYTE CBSoundMgr::GetVolumePercent(TSoundType Type)
{
	int Volume;
	switch(Type)
	{
	case SOUND_SFX:
		Volume = m_VolumeSFX;
		break;
	case SOUND_SPEECH:
		Volume = m_VolumeSpeech;
		break;
	case SOUND_MUSIC:
		Volume = m_VolumeMusic;
		break;
	}
	
	return (BYTE)Volume;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::SetMasterVolumePercent(BYTE Percent)
{
	m_VolumeMaster = Percent;
	// TODO how does SDL_mixer handle a master volume setting?
	// BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, (DWORD)(10000.0f / 100.0f * (float)Percent));
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
BYTE CBSoundMgr::GetMasterVolumePercent()
{
	// TODO how does SDL_mixer handle a master volume setting?
	// DWORD val = BASS_GetConfig(BASS_CONFIG_GVOL_STREAM);
	//	return (float) val / 10000.0f * 100.0f;
	return 100.0f;
}



//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::PauseAll(bool IncludingMusic)
{
	for (int i=0; i<m_Sounds.GetSize(); i++)
	{
		if(m_Sounds[i]->IsPlaying() && (m_Sounds[i]->m_Type != SOUND_MUSIC || IncludingMusic))
		{
			m_Sounds[i]->Pause();
			m_Sounds[i]->m_FreezePaused = true;
		}
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundMgr::ResumeAll()
{
	for (int i=0; i<m_Sounds.GetSize(); i++)
	{
		if (m_Sounds[i]->m_FreezePaused)
		{
			m_Sounds[i]->Resume();
			m_Sounds[i]->m_FreezePaused = false;
		}
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
float CBSoundMgr::PosToPan(int X, int Y)
{
	/*
	 * This is tricky to do right. Scenes could be scrolling (thus bigger than rendering width)
	 * and even then objects that emit sound could be "outside" the scene.
	 *
	 * As a compromise, the range where panning is applied is defined from
	 * (-0.5 * width) .. 0 .. (+1.5 * width).
	 *
	 * Because the sound library might simply ignore values out of range, extreme
	 * values are truncated.
	 */
	float width = (float) Game->m_Renderer->m_Width;
	float relPos = ((float) X + (0.5f * width)) / (width * 2.0f);

	// saturate
	if (relPos < 0.0f) {
		relPos = 0.0f;
	}
	if (relPos > 1.0f) {
		relPos = 1.0f;
	}

	float minPan = -1.0f;
	float maxPan = 1.0f;

	return minPan + relPos * (maxPan - minPan);
}

/////////////////////////////////////////////////////////////////////////
void CBSoundMgr::InvalidateChannels(int channel_number, bool streamed)
{
	Game->LOG(0, "Invalidate %s channel with number %d.", (streamed == true) ? "streamed" : "non-streamed", channel_number);
	for (int i=0; i < m_Sounds.GetSize(); i++)
	{
		m_Sounds[i]->InvalidateChannelNumber(channel_number);
	}
}

/////////////////////////////////////////////////////////////////////////
void CBSoundMgr::SDLMixer_channelDoneCallback(void *userdata, int channel_number)
{
	CBSoundMgr *obj = static_cast<CBSoundMgr*>(userdata);
	obj->InvalidateChannels(channel_number, false);
}

void CBSoundMgr::SDLMixer_musicFinishedHook(void *userdata, Mix_Music *music, int channel)
{
	CBSoundMgr *obj = static_cast<CBSoundMgr*>(userdata);
	obj->InvalidateChannels(channel, true);
}
