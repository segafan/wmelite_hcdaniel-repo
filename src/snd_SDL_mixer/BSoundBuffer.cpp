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
#include "BSoundBuffer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_NONSTREAMED_FILE_SIZE 1024*1024

//////////////////////////////////////////////////////////////////////////
CBSoundBuffer::CBSoundBuffer(CBGame* inGame):CBBase(inGame)
{
	m_chunk = NULL;
	m_music = NULL;
	m_rwops = NULL;
	m_currChannel = -1;
        

	m_Streamed = false;
	m_Filename = NULL;
	m_File = NULL;
	m_PrivateVolume = 100;
	m_cachedVolume = 100;

	m_Looping = false;
	m_LoopStart = 0;

	m_Type = SOUND_SFX;

	m_FreezePaused = false;

	m_ReverbContext.hInstance = NULL;

	m_EchoContext.inBuffer = NULL;
}


//////////////////////////////////////////////////////////////////////////
CBSoundBuffer::~CBSoundBuffer()
{
	Stop();

	if (m_ReverbContext.hInstance != NULL)
	{
		free(m_ReverbContext.InFrames32);
		free(m_ReverbContext.OutFrames32);
		Reverb_free(&m_ReverbContext);
		m_ReverbContext.hInstance = NULL;
	}

	if (m_EchoContext.inBuffer != NULL)
	{
		Echo_free(&m_EchoContext);
		m_EchoContext.inBuffer = NULL;
	}

	if (m_chunk)
	{
		Mix_FreeChunk(m_chunk);
		m_chunk = NULL;
	}

	if (m_music)
	{
		Mix_FreeMusic(m_music);
		m_music = NULL;
	}
  
	if (m_rwops)
	{
		SDL_FreeRW(m_rwops);
		m_rwops = NULL;
	}

	if(m_File)
	{
		Game->m_FileManager->CloseFile(m_File);
		m_File = NULL;
	}

	m_currChannel = -1;

	// TODO could try de-allocating channels here...

	SAFE_DELETE_ARRAY(m_Filename);
}


//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::SetStreaming(bool Streamed, DWORD NumBlocks, DWORD BlockSize)
{
	m_Streamed = Streamed;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::LoadFromFile(const char* Filename, bool ForceReload)
{
	if (m_ReverbContext.hInstance != NULL)
	{
		free(m_ReverbContext.InFrames32);
		free(m_ReverbContext.OutFrames32);
		Reverb_free(&m_ReverbContext);
		m_ReverbContext.hInstance = NULL;
	}

	if (m_EchoContext.inBuffer != NULL)
	{
		Echo_free(&m_EchoContext);
		m_EchoContext.inBuffer = NULL;
	}

	if (m_chunk)
	{
		Mix_FreeChunk(m_chunk);
		m_chunk = NULL;
	}

	if (m_music)
	{
		Mix_FreeMusic(m_music);
		m_music = NULL;
	}

	if (m_rwops)
	{
		SDL_FreeRW(m_rwops);
		m_rwops = NULL;
	}

	if (m_File) Game->m_FileManager->CloseFile(m_File);
	
	m_currChannel = -1;
	m_cachedVolume = 100;

	m_File = Game->m_FileManager->OpenFile(Filename);
	if (!m_File)
	{
		Game->LOG(0, "Error opening sound file '%s'", Filename);
		return E_FAIL;
	}
	
	m_rwops = SDL_AllocRW();
	m_rwops->size = CBSoundBuffer::FileSizeImpl;
	m_rwops->seek = CBSoundBuffer::FileSeekImpl;
	m_rwops->read = CBSoundBuffer::FileReadImpl;
	m_rwops->write = CBSoundBuffer::FileWriteImpl;
	m_rwops->close = CBSoundBuffer::FileCloseImpl;
	m_rwops->type = SDL_RWOPS_UNKNOWN;
	m_rwops->hidden.unknown.data1 = m_File;

	// check whether to load the file as music (streamed) or chunk (not streamed)
	if ((m_Streamed == true) || (FileSizeImpl(m_rwops) > MAX_NONSTREAMED_FILE_SIZE))
	{
		m_music = Mix_LoadMUS_RW(m_rwops, 0);
		if (!m_music)
		{
			Game->LOG(0, "SDL_mixer streamed error: %d while loading '%s'", Mix_GetError(), Filename);
			return E_FAIL;
		}

		Game->LOG(0, "SDL_mixer streamed file %s raw size %d.", Filename, Mix_GetMusicDuration(m_music));
	}
	else
	{
		m_chunk = Mix_LoadWAV_RW(m_rwops, 0);
		if (!m_chunk)
		{
			Game->LOG(0, "SDL_mixer non-streamed error: %d while loading '%s'", Mix_GetError(), Filename);
			return E_FAIL;
		}

		Game->LOG(0, "SDL_mixer non-streamed file %s raw chunk size %d.", Filename, m_chunk->alen);
	}

	CBUtils::SetString(&m_Filename, Filename);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::Play(bool Looping, DWORD StartSample)
{
	SetLooping(Looping);

	if (m_chunk)
	{
		m_currChannel = Mix_PlayChannelTimed(-1, m_chunk, (Looping == true) ? -1 : 0, -1, m_LoopStart);
		Game->LOG(0, "Started non-streaming play.");
	}
	if (m_music)
	{
		m_currChannel = Mix_PlayMusicCh(m_music, (Looping == true) ? -1 : 0, -1);
		Game->LOG(0, "Started streaming play.");
	}

	Game->LOG(0, "Allocated channel for file %s = %d.", m_Filename, m_currChannel);
	
	// delete all previously registered effects
	Mix_UnregisterAllEffects(m_currChannel);

	// set volume to the last cached one (volume could have been set before channel number was known)
	SetVolume(m_cachedVolume);

	// reset panning of channel to default
	Mix_SetPanning(m_currChannel, 127, 127);

	// register reverb if channel number was not known during reverb config
	// TODO do not double-register!!!
	if (m_ReverbContext.hInstance != NULL)
	{
		if (!Mix_RegisterEffect(m_currChannel, &ReverbEffectFunction, &ReverbEffectDoneFunction, (void *) this))
		{
			Game->LOG(0, "Register reverb effect error!");
			free(m_ReverbContext.OutFrames32);
			free(m_ReverbContext.InFrames32);
			Reverb_free(&m_ReverbContext);
			m_ReverbContext.hInstance = NULL;
		}
	}

	// register echo if channel number was not known during echo config
	// TODO do not double-register!!!
	if (m_EchoContext.inBuffer != NULL)
	{
		if (!Mix_RegisterEffect(m_currChannel, &EchoEffectFunction, &EchoEffectDoneFunction, (void *) this))
		{
			Game->LOG(0, "Register echo effect error!");
			Echo_free(&m_EchoContext);
			m_EchoContext.inBuffer = NULL;
		}
	}

	if ((m_chunk) || (m_music))
	{
		// re-allocate enough mixer channels dynamically
		if (m_currChannel >= 0) 
		{
			int allocated_channels = Mix_AllocateChannels(-1);
			if ((allocated_channels - m_currChannel) < 4)
			{
				Game->LOG(0, "Reallocating SDL_mixer channels: Total %d current %d.", allocated_channels, m_currChannel);
				Mix_AllocateChannels(allocated_channels + 4);
			}
		}
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::SetLooping(bool looping)
{
	m_Looping = looping;

	// change the looping parameter of the already playing stream
	if ((m_chunk) && (m_currChannel >= 0))
	{
		Mix_SetLoops(m_currChannel, (looping == true) ? -1 : 0);
	}
	if ((m_music) && (m_currChannel >= 0))
	{
		// TODO
		Game->LOG(0, "FIXME: Set looping for already playing music not yet implemented.");
	}
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::Resume()
{
	if ((m_chunk) && (m_currChannel >= 0))
	{
		Mix_Resume(m_currChannel);
	}
	if ((m_music) && (m_currChannel >= 0))
	{
		Mix_ResumeMusicCh(m_currChannel);
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::Stop()
{
	if ((m_chunk) && (m_currChannel >= 0))
	{
		Game->LOG(0, "Non-buffered channel %d stopped!", m_currChannel);
		Mix_HaltChannel(m_currChannel);
		m_currChannel = -1; // channel number is invalid from now on
	}
	if ((m_music) && (m_currChannel >= 0))
	{
		Game->LOG(0, "Buffered channel %d stopped!", m_currChannel);
		Mix_HaltMusicCh(m_music);
		m_currChannel = -1; // channel number is invalid from now on
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::Pause()
{
	if ((m_chunk) && (m_currChannel >= 0))
	{
		 Mix_Pause(m_currChannel);
	}
	if ((m_music) && (m_currChannel >= 0))
	{
		Mix_PauseMusicCh(m_currChannel);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
DWORD CBSoundBuffer::GetLength()
{
	int play_length_ms = 0;	

	if (m_chunk)
	{
		play_length_ms = Mix_GetPlayLength(m_chunk);
	}
	if (m_music)
	{
		play_length_ms = Mix_GetMusicDuration(m_music);
	}
	
	Game->LOG(0, "File %s play length %d ms.", m_Filename, play_length_ms);

	return play_length_ms;
}


//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::SetType(TSoundType Type)
{
	m_Type = Type;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetVolume(int Volume)
{
	m_cachedVolume = Volume;
	
	// compute the "weighted" value (volume relatively to TSoundType category's volume)
	float resultingVolumePerCent = ((float)Volume / 100.0f) * m_PrivateVolume;

	resultingVolumePerCent *= 1.2f; // SDL_mixer volume ranges from 0 to 127

	if (m_currChannel == -1)
	{
		// Game->LOG(0, "Caching volume %d for file '%s' because channel not yet allocated.", Volume, m_Filename);
	}
	else
	{
		// Game->LOG(0, "Setting volume on channel %d request %d m_Private %d applied %.2f.", m_currChannel, Volume, m_PrivateVolume, resultingVolumePerCent);

		if ((m_chunk) && (m_currChannel >= 0))
		{
			Mix_Volume(m_currChannel, (int) resultingVolumePerCent);
		}
		if ((m_music) && (m_currChannel >= 0))
		{
			Mix_Volume(m_currChannel, (int) resultingVolumePerCent);

			// hmmmm
			// Mix_VolumeMusicCh((int) resultingVolumePerCent, m_currChannel);
		}
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetPrivateVolume(int Volume)
{
	m_PrivateVolume = Volume;

	switch(m_Type)
	{
	case SOUND_SFX:
		Volume = Game->m_SoundMgr->m_VolumeSFX;
		break;
	case SOUND_SPEECH:
		Volume = Game->m_SoundMgr->m_VolumeSpeech;
		break;
	case SOUND_MUSIC:
		Volume = Game->m_SoundMgr->m_VolumeMusic;
		break;
	}
	return SetVolume(Volume);
}


//////////////////////////////////////////////////////////////////////////
bool CBSoundBuffer::IsPlaying()
{
	bool isPlaying = false;	
	if ((m_chunk) && (m_currChannel >= 0))
	{
		isPlaying = Mix_Playing(m_currChannel) != 0;
	}
	if ((m_music) && (m_currChannel >= 0))
	{
		isPlaying = Mix_PlayingMusicCh(m_currChannel) != 0;
	}

	return m_FreezePaused || isPlaying;
}


//////////////////////////////////////////////////////////////////////////
DWORD CBSoundBuffer::GetPosition()
{
	int curr_position_ms = 0;	

	if ((m_chunk) && (m_currChannel >= 0))
	{
		curr_position_ms = Mix_SetPlayPosition(m_currChannel, -1);
	}
	if ((m_music) && (m_currChannel >= 0))
	{
		// TODO
		Game->LOG(0, "FIXME: Get position for music not yet implemented.");
	}

	Game->LOG(0, "File %s current position %d ms.", m_Filename, curr_position_ms);

	return curr_position_ms;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetPosition(DWORD Pos)
{
	if ((m_chunk) && (m_currChannel >= 0))
	{
		Mix_SetPlayPosition(m_currChannel, Pos);
	}
	if ((m_music) && (m_currChannel >= 0))
	{
		double position_s = ((double) Pos) / 1000.0f;
		Mix_SetMusicPositionCh(position_s, m_currChannel);
		Game->LOG(0, "Set music position is experimental!");
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetLoopStart(DWORD Pos)
{
	m_LoopStart = Pos;

	// sound looping shall start at the specified time in ms
	if ((m_chunk) && (m_currChannel >= 0))
	{
		Mix_SetLoopStart(m_currChannel, Pos);
	}
	if ((m_music) && (m_currChannel >= 0))
	{
		// TODO
		Game->LOG(0, "FIXME: Set loop start for music not yet implemented.");
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetPan(float Pan)
{
	// set pan (balance) attribute
	if (((m_chunk) || (m_music)) && (m_currChannel >= 0))
	{
		Uint8 left;
		Uint8 right;
		int result;

		left  = (Uint8) (127 * (1.0 - Pan));
		right = (Uint8) (127 * (1.0 + Pan));

		result = Mix_SetPanning(m_currChannel, left, right);

		// Game->LOG(0, "Setting pan on channel %d to left=%d right=%d, result=%d.", m_currChannel, left, right, result);

		if (m_music)
		{
			// TODO need to debug whether panning setting works on a music channel
			// Game->LOG(0, "Setting pan on music is experimental!");
		}
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::ApplyFX(TSFXType Type, float Param1, float Param2, float Param3, float Param4)
{
	int status; 

	// at first, remove all previously registered effects when the channel is already active
	if (m_currChannel >= 0)
	{
		while(Mix_UnregisterEffect(m_currChannel, &ReverbEffectFunction));
		while(Mix_UnregisterEffect(m_currChannel, &EchoEffectFunction));
	}

	switch(Type)
	{
	case SFX_ECHO:
		// TODO need to check whether context is already in use


		// attenuation = mean of param1 and param2, as float from 0..1
		// delay = mean of param3 and param 4 (echo delay in ms)
		// other params given by call to Mix_OpenAudio() in BSoundMgr

		status = Echo_init(&m_EchoContext, 2, 2, 44100, (((float) Param1 + (float) Param2) / 2.0f / 100.0f), (uint16_t) ((Param3 + Param4) / 2.0f));

		if (status != 0)
		{
			Game->LOG(0, "Echo init error=%d", status);
			return S_FALSE;
		}

		Game->LOG(0, "Echo effect generated. attenuation=%.2f delay=%d.", (((float) Param1 + (float) Param2) / 2.0f / 100.0f), (uint16_t) ((Param3 + Param4) / 2.0f));

		break;

	case SFX_REVERB_PRESET:
		// TODO need to check whether context is already in use

		uint32_t replyCount;
		char replydata[8];

		m_ReverbContext.hInstance  = NULL;
		m_ReverbContext.boolAuxiliary  = 0;

	    m_ReverbContext.boolPreset     = 1;
	    m_ReverbContext.curPreset  = REVERB_PRESET_LAST + 1;

	    switch (int (Param1))
	    {
	    case 0:
	    	m_ReverbContext.nextPreset = REVERB_PRESET_NONE;
	    	break;
	    case 1:
	    	m_ReverbContext.nextPreset = REVERB_PRESET_SMALLROOM;
	    	break;
	    case 2:
	    	m_ReverbContext.nextPreset = REVERB_PRESET_MEDIUMROOM;
	    	break;
	    case 3:
	    	m_ReverbContext.nextPreset = REVERB_PRESET_LARGEROOM;
	    	break;
	    case 4:
	    	m_ReverbContext.nextPreset = REVERB_PRESET_MEDIUMHALL;
	    	break;
	    case 5:
	    	m_ReverbContext.nextPreset = REVERB_PRESET_LARGEHALL;
	    	break;
	    case 6:
	    	m_ReverbContext.nextPreset = REVERB_PRESET_PLATE;
	    	break;
	    default:
	    	m_ReverbContext.nextPreset = REVERB_PRESET_NONE;
	    	break;
	    }

		Game->LOG(0, "====> Next preset=%d.\n", m_ReverbContext.nextPreset);

		status = Reverb_init(&m_ReverbContext);

		if (status != 0)
		{
			Game->LOG(0, "Reverb init error=%d", status);
			return S_FALSE;
		}

		m_ReverbContext.config.outputCfg.accessMode = EFFECT_BUFFER_ACCESS_WRITE;

	    m_ReverbContext.InFrames32  = (LVM_INT32 *)malloc(LVREV_MAX_FRAME_SIZE * sizeof(LVM_INT32) * 2);

		if (m_ReverbContext.InFrames32 == NULL)
		{
			Game->LOG(0, "Reverb malloc failed (1)!");
			Reverb_free(&m_ReverbContext);
			m_ReverbContext.hInstance = NULL;
			return S_FALSE;
		}
		else
		{
			// Game->LOG(0, "Malloc inframes addr 0x%08X!", (uint32_t*) m_context.InFrames32);
		}

	    m_ReverbContext.OutFrames32 = (LVM_INT32 *)malloc(LVREV_MAX_FRAME_SIZE * sizeof(LVM_INT32) * 2);

		if (m_ReverbContext.OutFrames32 == NULL)
		{
			Game->LOG(0, "Reverb malloc failed (2)!");
			free(m_ReverbContext.InFrames32);
			Reverb_free(&m_ReverbContext);
			m_ReverbContext.hInstance = NULL;
			return S_FALSE;
		}
		else
		{
			// Game->LOG(0, "Malloc outframes addr 0x%08X!", (uint32_t*) m_context.OutFrames32);
		}

	    replyCount = sizeof(int);

	    status = Reverb_command(&m_ReverbContext, EFFECT_CMD_ENABLE, 0, NULL, &replyCount, replydata);

		if (status != 0)
		{
			Game->LOG(0, "Reverb command error=%d", status);
			free(m_ReverbContext.OutFrames32);
			free(m_ReverbContext.InFrames32);
			Reverb_free(&m_ReverbContext);
			m_ReverbContext.hInstance = NULL;
			return S_FALSE;
		}

		if (m_currChannel >= 0)
		{
			if (!Mix_RegisterEffect(m_currChannel, &ReverbEffectFunction, &ReverbEffectDoneFunction, (void *) this))
			{
				Game->LOG(0, "Register reverb effect error!");
				free(m_ReverbContext.OutFrames32);
				free(m_ReverbContext.InFrames32);
				Reverb_free(&m_ReverbContext);
				m_ReverbContext.hInstance = NULL;
				return S_FALSE;
			}
		}

	//	Game->LOG(0, "Preset reverb active, ptr=0x%08X.", (uint32_t) m_pContext);
		Game->LOG(0, "Preset reverb active.");


		break;

// freely configurable reverb not yet implemented
//	case SFX_REVERB:
	default:
		// deallocate all effect data
		if (m_ReverbContext.hInstance != NULL)
		{
			free(m_ReverbContext.InFrames32);
			free(m_ReverbContext.OutFrames32);
			Reverb_free(&m_ReverbContext);
			m_ReverbContext.hInstance = NULL;
		}

		if (m_EchoContext.inBuffer != NULL)
		{
			Echo_free(&m_EchoContext);
			m_EchoContext.inBuffer = NULL;
		}

		break;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::InvalidateChannelNumber(int finished_channel)
{
    if (m_currChannel >= 0) {
        if (m_currChannel == finished_channel) {
            m_currChannel = -1;
        }
    }
}

//////////////////////////////////////////////////////////////////////////
Sint64 CBSoundBuffer::FileSizeImpl(SDL_RWops *ops)
{
	CBFile* file = static_cast<CBFile*>(ops->hidden.unknown.data1);
	return file->GetSize();
}

//////////////////////////////////////////////////////////////////////////
Sint64 CBSoundBuffer::FileSeekImpl(SDL_RWops *ops, Sint64 offset, int whence)
{
    CBFile* file = static_cast<CBFile*>(ops->hidden.unknown.data1);
    TSeek Origin = SEEK_TO_BEGIN;
        if (whence == RW_SEEK_CUR) {
        Origin = SEEK_TO_CURRENT;
    }
    if (whence == RW_SEEK_END) {
        Origin = SEEK_TO_END;
    }
    file->Seek(offset, Origin);

    return file->GetPos();
}

//////////////////////////////////////////////////////////////////////////
size_t CBSoundBuffer::FileReadImpl(SDL_RWops *ops, void *buffer, size_t size, size_t nmemb)
{
	CBFile* file = static_cast<CBFile*>(ops->hidden.unknown.data1);
	DWORD oldPos = file->GetPos();
	file->Read(buffer, (nmemb * size));
	return file->GetPos() - oldPos;
}

//////////////////////////////////////////////////////////////////////////
size_t CBSoundBuffer::FileWriteImpl(SDL_RWops *ops, const void *buffer, size_t size, size_t nmemb)
{
  return 0;
}

//////////////////////////////////////////////////////////////////////////
int CBSoundBuffer::FileCloseImpl(SDL_RWops *ops)
{
  return 0;
}

//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::ReverbEffectFunction(int chan, void *stream, int len, void *udata)
{
	int status;
	ReverbContext *ctx;
	audio_buffer_t audio_in;
	audio_buffer_t audio_out;
	CBSoundBuffer *obj; 
	DWORD currLength;
	DWORD totalLength;

	obj = static_cast<CBSoundBuffer*>(udata);

	obj->Game->LOG(0, "Reverb process chan=%d len=%d.", chan, len);

	ctx = &(obj->m_ReverbContext);

	currLength = 0;
	totalLength = 0;

	while (totalLength < len)
	{
		currLength = (len - totalLength);
		if ((currLength / 4) > LVREV_MAX_FRAME_SIZE)
		{
			currLength = LVREV_MAX_FRAME_SIZE * 4;
		} 		

		audio_in.raw = ((uint8_t *) stream) + totalLength;
		// memcpy(inbuf, buffer, length);

		audio_out.raw = ((uint8_t *) stream) + totalLength;

		// TODO need to properly adjust this!
		audio_in.frameCount = currLength / 4;
		audio_out.frameCount = currLength / 4;

		// obj->Game->LOG(0, "Frame count=%d.", audio_out.frameCount);

		status = Reverb_process(ctx, &audio_in, &audio_out);

		if (status != 0)
		{
			obj->Game->LOG(0, "Reverb process error=%d", status);
		}

		totalLength += currLength;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::ReverbEffectDoneFunction(int chan, void *udata)
{
	CBSoundBuffer *obj;

	obj = static_cast<CBSoundBuffer*>(udata);

	obj->Game->LOG(0, "Reverb done callback, freeing reverb effect of channel %d.", chan);

	free(obj->m_ReverbContext.InFrames32);
	free(obj->m_ReverbContext.OutFrames32);
	Reverb_free(&(obj->m_ReverbContext));
	obj->m_ReverbContext.hInstance = NULL;
}

//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::EchoEffectFunction(int chan, void *stream, int len, void *udata)
{
	CBSoundBuffer *obj;
	int status;

	obj = static_cast<CBSoundBuffer*>(udata);

	obj->Game->LOG(0, "Echo process chan=%d len=%d.", chan, len);

	status = Echo_process(&(obj->m_EchoContext), (uint8_t*) stream, (uint32_t) len);

	// testing
	// memset(stream, 0, len);

	if (status != 0)
	{
		obj->Game->LOG(0, "Echo process error=%d", status);
	}

}

//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::EchoEffectDoneFunction(int chan, void *udata)
{
	CBSoundBuffer *obj;

	obj = static_cast<CBSoundBuffer*>(udata);

	obj->Game->LOG(0, "Echo done callback, freeing echo effect of channel %d.", chan);

	Echo_free(&(obj->m_EchoContext));
	obj->m_EchoContext.inBuffer = NULL;
}
