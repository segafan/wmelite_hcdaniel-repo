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

#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define MAX_NONSTREAMED_FILE_SIZE 1024*1024

//////////////////////////////////////////////////////////////////////////
CBSoundBuffer::CBSoundBuffer(CBGame* inGame):CBBase(inGame)
{
	m_Stream = NULL;
	m_Sync = NULL;

	m_Streamed = false;
	m_Filename = NULL;
	m_File = NULL;
	m_PrivateVolume = 100;

	m_Looping = false;
	m_LoopStart = 0;

	m_Type = SOUND_SFX;

	m_FreezePaused = false;

#ifdef USE_BASS_FX
	m_EffectHandle = 0;
#endif

#ifdef USE_LIBEFFECTS_REVERB
	m_context.hInstance = NULL;
	m_BASS_DSP_handle = 0;
#endif
}


//////////////////////////////////////////////////////////////////////////
CBSoundBuffer::~CBSoundBuffer()
{
	Stop();

	if (m_Stream)
	{
		BASS_StreamFree(m_Stream);
		m_Stream = NULL;
	}

	if(m_File)
	{
		Game->m_FileManager->CloseFile(m_File);
		m_File = NULL;
	}

#ifdef USE_LIBEFFECTS_REVERB
	if (m_context.hInstance != NULL)
	{
		free(m_context.InFrames32);
		free(m_context.OutFrames32);
		Reverb_free(&m_context);
		m_context.hInstance = NULL;
	}
#endif


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
	if (m_Stream)
	{
		BASS_StreamFree(m_Stream);
		m_Stream = NULL;
	}

	if (m_File) Game->m_FileManager->CloseFile(m_File);
	
	m_File = Game->m_FileManager->OpenFile(Filename);
	if (!m_File)
	{
		Game->LOG(0, "Error opening sound file '%s'", Filename);
		return E_FAIL;
	}
	
	BASS_FILEPROCS fileProc;
	fileProc.close = CBSoundBuffer::FileCloseProc;
	fileProc.read = CBSoundBuffer::FileReadProc;
	fileProc.seek = CBSoundBuffer::FileSeekProc;
	fileProc.length = CBSoundBuffer::FileLenProc;

	m_Stream = BASS_StreamCreateFileUser(STREAMFILE_NOBUFFER, 0, &fileProc, (void*)m_File);
	if (!m_Stream)
	{
		Game->LOG(0, "BASS error: %d while loading '%s'", BASS_ErrorGetCode(), Filename);
		return E_FAIL;
	}

	CBUtils::SetString(&m_Filename, Filename);

#ifdef USE_BASS_FX
	if (m_EffectHandle != 0)
	{
		BASS_ChannelRemoveFX(m_Stream, m_EffectHandle);
		m_EffectHandle = 0;
	}
#endif

#ifdef USE_LIBEFFECTS_REVERB
	if (m_BASS_DSP_handle != 0)
	{
		if (BASS_ChannelRemoveDSP(m_Stream, m_BASS_DSP_handle) == FALSE)
		{
			Game->LOG(0, "BASS error: %d while removing ond DSP effect from '%s'", BASS_ErrorGetCode(), Filename);
		}
		m_BASS_DSP_handle = 0;
	}
	if (m_context.hInstance != NULL)
	{
		free(m_context.InFrames32);
		free(m_context.OutFrames32);
		Reverb_free(&m_context);
		m_context.hInstance = NULL;
	}
#endif

	/*
	HRESULT res;
	bool NewlyCreated = false;

	if(!m_SoundBuffer || ForceReload || m_Streamed){
		if(!m_File) m_File = Game->m_FileManager->OpenFile(Filename);
		if(!m_File){
			Game->LOG(0, "Error opening sound file '%s'", Filename);
			return E_FAIL;
		}
		// switch to streamed for big files
		if(!m_Streamed && (m_File->GetSize() > MAX_NONSTREAMED_FILE_SIZE && !Game->m_ForceNonStreamedSounds)) SetStreaming(true);
	}

	// create buffer
	if(!m_SoundBuffer){
		NewlyCreated = true;
		
		res = InitializeBuffer(m_File);
		if(FAILED(res)){
			Game->LOG(res, "Error creating sound buffer for file '%s'", Filename);
			return res;
		}
	}



	// store filename
	if(!m_Filename){
		m_Filename = new char[strlen(Filename)+1];
		strcpy(m_Filename, Filename);
	}

	// close file (if not streaming)
	if(!m_Streamed && m_File){
		Game->m_FileManager->CloseFile(m_File);
		m_File = NULL;
	}
	*/

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::Play(bool Looping, DWORD StartSample)
{
	if (m_Stream)
	{
		SetLooping(Looping);
		BASS_ChannelPlay(m_Stream, FALSE);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::SetLooping(bool looping)
{
	m_Looping = looping;

	if (m_Stream)
	{
		BASS_ChannelFlags(m_Stream, looping ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP);
	}
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::Resume()
{
	if (m_Stream)
	{
		BASS_ChannelPlay(m_Stream, FALSE);
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::Stop()
{
	if (m_Stream)
	{
		BASS_ChannelStop(m_Stream);
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::Pause()
{
	if (m_Stream)
	{
		BASS_ChannelPause(m_Stream);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
DWORD CBSoundBuffer::GetLength()
{
	QWORD len = BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE);
	return 1000 * BASS_ChannelBytes2Seconds(m_Stream, len);
}


//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::SetType(TSoundType Type)
{
	m_Type = Type;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetVolume(int Volume)
{
	if (m_Stream)
	{
		// compute the "weighted" value (volume relatively to TSoundType category's volume)
		float resultingVolumePerCent = ((float)Volume / 100.0f) * m_PrivateVolume;

		/*
		 * Repeat the volume computation of WME so that WMElite mimics WME.
		 */
		double attenuate = pow(1 - (resultingVolumePerCent / 100.0f), 3);
		int myFinalVolume = int(-10000 * attenuate);
		/*
		 * Now that the volume is in 0,01dB steps, reverse-compute percentage (sort of, values are experimental)
		 */
		float resultingVolumeBass = (float) pow(10.0f, (myFinalVolume / 2000.0f));

		BASS_ChannelSetAttribute(m_Stream, BASS_ATTRIB_VOL, resultingVolumeBass);
		// Game->LOG(0, "BASS_Setvolume for stream %s occurred, volume=%d, privVolume=%d, wmevalue=%d resulting float value=%f.\n", m_Filename, Volume, m_PrivateVolume, myFinalVolume, resultingVolumeBass);
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
	return m_FreezePaused || BASS_ChannelIsActive(m_Stream) == BASS_ACTIVE_PLAYING;
}


//////////////////////////////////////////////////////////////////////////
DWORD CBSoundBuffer::GetPosition()
{
	if (m_Stream)
	{
		QWORD len = BASS_ChannelGetPosition(m_Stream, BASS_POS_BYTE);
		return 1000 * BASS_ChannelBytes2Seconds(m_Stream, len);
	}
	else return 0;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetPosition(DWORD Pos)
{
	if (m_Stream)
	{
		QWORD pos = BASS_ChannelSeconds2Bytes(m_Stream, (float)Pos / 1000.0f);		
		BASS_ChannelSetPosition(m_Stream, pos, BASS_POS_BYTE);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetLoopStart(DWORD Pos)
{
	m_LoopStart = Pos;

	if (m_Stream)
	{
		if (m_Sync)
		{
			BASS_ChannelRemoveSync(m_Stream, m_Sync);
			m_Sync = NULL;
		}
		if (m_LoopStart > 0)
		{
			QWORD len = BASS_ChannelGetLength(m_Stream, BASS_POS_BYTE);
			m_Sync = BASS_ChannelSetSync(m_Stream, BASS_SYNC_POS | BASS_SYNC_MIXTIME, len, CBSoundBuffer::LoopSyncProc, (void*)this);
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::LoopSyncProc(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	CBSoundBuffer* soundBuf = static_cast<CBSoundBuffer*>(user);
	QWORD pos = BASS_ChannelSeconds2Bytes(channel, (float)soundBuf->GetLoopStart() / 1000.0f);
	
	if (!BASS_ChannelSetPosition(channel, pos, BASS_POS_BYTE))
		BASS_ChannelSetPosition(channel, 0, BASS_POS_BYTE);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::SetPan(float Pan)
{
	if (m_Stream)
	{
		BASS_ChannelSetAttribute(m_Stream, BASS_ATTRIB_PAN, Pan);
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSoundBuffer::ApplyFX(TSFXType Type, float Param1, float Param2, float Param3, float Param4)
{
	switch(Type)
	{
	case SFX_ECHO:
#ifdef USE_BASS_FX
		if (m_EffectHandle != 0)
		{
			BASS_ChannelRemoveFX(m_Stream, m_EffectHandle);
		}
		m_EffectHandle = BASS_ChannelSetFX(
			m_Stream,
			BASS_FX_BFX_ECHO4,
			0);

		if (m_EffectHandle != 0) 
		{
			BASS_BFX_ECHO4 params;
			/*
			 * try to be as close as possible to the DSFX echo, it will never be perfect though
			 * 
			 * fDryMix = reverse of fWetDryMix (scaled from 0..100 into 0..1)
			 * fWetMix = fWetDryMix (converted from logarithmic to linear) 
			 * fDelay  = mean of fLeftDelay and fRightDelay (converted from milliseconds to seconds)
			 * ignored = fFeedback (the BASS feedback parameter is used to create reverb)
			 */
			params.fDryMix = ((100.0f - Param1) / 100.0f);
			params.fWetMix = (Param1 / 100.0f);
			params.fFeedback = 0;
			params.fDelay = (Param3 + Param4) / 2.0f / 1000.0f;
			params.bStereo = false;
			params.lChannel = BASS_BFX_CHANALL;

			Game->LOG(0, "Echo params: Dry=%.2f Wet=%.2f Fedback=%.2f Delay=%.2f", params.fDryMix, params.fWetMix, params.fFeedback, params.fDelay);

			BOOL status = BASS_FXSetParameters(
				m_EffectHandle,
				(void *) &params);

			Game->LOG(0, "Echo status=%s", (status == TRUE) ? "OK" : "ERROR");
			if (status == FALSE)
			{
				Game->LOG(0, "Echo error=%d", BASS_ErrorGetCode());
			}
		}
#endif
		break;

	case SFX_REVERB:
#ifdef USE_BASS_FX
		if (m_EffectHandle != 0)
		{
			BASS_ChannelRemoveFX(m_Stream, m_EffectHandle);
		}	
		m_EffectHandle = BASS_ChannelSetFX(
			m_Stream,
			BASS_FX_BFX_ECHO4,
			0);
		if (m_EffectHandle != 0) 
		{
			BASS_BFX_ECHO4 params;
			/*
			 * try to be as close as possible to the DSFX reverb, it will never be perfect though
			 * 
			 * fDryMix = fInGain (converted from logarithmic to linear)
			 * fWetMix = fReverbMix (converted from logarithmic to linear) multiplied ("attenuated") with fDryMix
			 * fDelay  = fReverbTime (converted from milliseconds to seconds)
			 * ignored = fHighFreqRTRatio (no equivalent in BASS)
			 */
			params.fDryMix = pow(10.0f, (Param1 / 10.0f));
			params.fWetMix = pow(10.0f, (Param2 / 10.0f)) * params.fDryMix;
			params.fFeedback = 0.7f;
			params.fDelay = ((Param3 / 1000.0f) / 10.0f);

			// test a different approach, where only the feedback parameter is affected by the fReverbTime
			params.fDryMix = 0.999f;
			params.fWetMix = 0.7f;
			params.fFeedback = 0.5f + ((Param3 / 3000.0f) * 0.2f);
			params.fDelay = 0.005f;

			params.bStereo = false;
			params.lChannel = BASS_BFX_CHANALL;

			Game->LOG(0, "Reverb params: Dry=%.2f Wet=%.2f Fedback=%.2f Delay=%.2f", params.fDryMix, params.fWetMix, params.fFeedback, params.fDelay);

			BOOL status = BASS_FXSetParameters(
				m_EffectHandle,
				(void *) &params);

			Game->LOG(0, "Reverb status=%s", (status == TRUE) ? "OK" : "ERROR");
			if (status == FALSE)
			{
				Game->LOG(0, "Reverb error=%d", BASS_ErrorGetCode());
			}
		}
#endif
		break;

	case SFX_REVERB_PRESET:
#ifdef USE_LIBEFFECTS_REVERB
		int status;
		uint32_t replyCount;
		char replydata[8];

		m_context.hInstance  = NULL;
		m_context.boolAuxiliary  = 0;

	    m_context.boolPreset     = 1;
	    m_context.curPreset  = REVERB_PRESET_LAST + 1;

	    switch (int (Param1))
	    {
	    case 0:
	    	m_context.nextPreset = REVERB_PRESET_NONE;
	    	break;
	    case 1:
	    	m_context.nextPreset = REVERB_PRESET_SMALLROOM;
	    	break;
	    case 2:
	    	m_context.nextPreset = REVERB_PRESET_MEDIUMROOM;
	    	break;
	    case 3:
	    	m_context.nextPreset = REVERB_PRESET_LARGEROOM;
	    	break;
	    case 4:
	    	m_context.nextPreset = REVERB_PRESET_MEDIUMHALL;
	    	break;
	    case 5:
	    	m_context.nextPreset = REVERB_PRESET_LARGEHALL;
	    	break;
	    case 6:
	    	m_context.nextPreset = REVERB_PRESET_PLATE;
	    	break;
	    default:
	    	m_context.nextPreset = REVERB_PRESET_NONE;
	    	break;
	    }

		Game->LOG(0, "====> Next preset=%d.\n", m_context.nextPreset);

		status = Reverb_init(&m_context);

		if (status != 0)
		{
			Game->LOG(0, "Reverb init error=%d", status);
		}

		m_context.config.outputCfg.accessMode = EFFECT_BUFFER_ACCESS_WRITE;

	    m_context.InFrames32  = (LVM_INT32 *)malloc(LVREV_MAX_FRAME_SIZE * sizeof(LVM_INT32) * 2);

		if (m_context.InFrames32 == NULL)
		{
			Game->LOG(0, "Reverb malloc failed!");
		}
		else
		{
			Game->LOG(0, "Malloc inframes addr 0x%08X!", (uint32_t*) m_context.InFrames32);
		}

	    m_context.OutFrames32 = (LVM_INT32 *)malloc(LVREV_MAX_FRAME_SIZE * sizeof(LVM_INT32) * 2);

		if (m_context.OutFrames32 == NULL)
		{
			Game->LOG(0, "Reverb malloc failed!");
		}
		else
		{
			Game->LOG(0, "Malloc outframes addr 0x%08X!", (uint32_t*) m_context.OutFrames32);
		}

	    replyCount = sizeof(int);

	    status = Reverb_command(&m_context, EFFECT_CMD_ENABLE, 0, NULL, &replyCount, replydata);

		if (status != 0)
		{
			Game->LOG(0, "Reverb command error=%d", status);
		}

		m_pContext = &m_context;

		m_BASS_DSP_handle = BASS_ChannelSetDSP(m_Stream, DSPProc, (void *) this, 0);

		if (m_BASS_DSP_handle == 0)
		{
			Game->LOG(0, "BASS error: %d while adding DSP effect", BASS_ErrorGetCode());
		}

		Game->LOG(0, "Preset reverb active, ptr=0x%08X.", (uint32_t) m_pContext);

#endif
		break;

	default:
#ifdef USE_BASS_FX
		if (m_EffectHandle != 0)
		{
			BASS_ChannelRemoveFX(m_Stream, m_EffectHandle);
			m_EffectHandle = 0;
		}
#endif

#ifdef USE_LIBEFFECTS_REVERB
	if (m_BASS_DSP_handle != 0)
	{
		if (BASS_ChannelRemoveDSP(m_Stream, m_BASS_DSP_handle) == FALSE)
		{
			Game->LOG(0, "BASS error: %d while removing old DSP effect", BASS_ErrorGetCode());
		}
		m_BASS_DSP_handle = 0;
	}
	if (m_context.hInstance != NULL)
	{
		free(m_context.InFrames32);
		free(m_context.OutFrames32);
		Reverb_free(&m_context);
		m_context.hInstance = NULL;
	}
#endif
		break;
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CBSoundBuffer::FileCloseProc(void* user)
{
	/*
	CBFile* file = static_cast<CBFile*>(user);
	file->Game->m_FileManager->CloseFile(file);
	*/
}

//////////////////////////////////////////////////////////////////////////
QWORD CBSoundBuffer::FileLenProc(void* user)
{
	CBFile* file = static_cast<CBFile*>(user);
	return file->GetSize();
}

//////////////////////////////////////////////////////////////////////////
DWORD CBSoundBuffer::FileReadProc(void *buffer, DWORD length, void* user)
{
	CBFile* file = static_cast<CBFile*>(user);
	DWORD oldPos = file->GetPos();
	file->Read(buffer, length);
	return file->GetPos() - oldPos;
}

//////////////////////////////////////////////////////////////////////////
BOOL CBSoundBuffer::FileSeekProc(QWORD offset, void *user)
{
	CBFile* file = static_cast<CBFile*>(user);
	return SUCCEEDED(file->Seek(offset));
}

//////////////////////////////////////////////////////////////////////////
#ifdef USE_LIBEFFECTS_REVERB
void CBSoundBuffer::DSPProc(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user)
{
	int status;
	ReverbContext *ctx;
	audio_buffer_t audio_in;
	audio_buffer_t audio_out;
	CBSoundBuffer *obj; 
	DWORD currLength;
	DWORD totalLength;
	// CBGame *Game;
	// char inbuf[100000];
	// char outbuf[100000];

	obj = static_cast<CBSoundBuffer*>(user);

	ctx = obj->m_pContext;
	// ctx = static_cast<ReverbContext*>(user);

	// Game = static_cast<CBGame*>(user);

	obj->Game->LOG(0, "In Callback! length=%d.", length);
	obj->Game->LOG(0, "In callback context is 0x%08X.", (uint32_t) ctx);

	obj->Game->LOG(0, "In callback inframes addr 0x%08X!", (uint32_t*) ctx->InFrames32);
	obj->Game->LOG(0, "In callback outframes addr 0x%08X!", (uint32_t*) ctx->OutFrames32);

	currLength = 0;
	totalLength = 0;

	while (totalLength < length)
	{
		currLength = (length - totalLength);
		if ((currLength / 4) > LVREV_MAX_FRAME_SIZE)
		{
			currLength = LVREV_MAX_FRAME_SIZE * 4;
		} 		

		audio_in.raw = ((uint8_t *) buffer) + totalLength;
		// memcpy(inbuf, buffer, length);

		audio_out.raw = ((uint8_t *) buffer) + totalLength;

		// TODO need to properly adjust this!
		audio_in.frameCount = currLength / 4;
		audio_out.frameCount = currLength / 4;

		obj->Game->LOG(0, "Frame count=%d.", audio_out.frameCount);

		status = Reverb_process(ctx, &audio_in, &audio_out);

		if (status != 0)
		{
			obj->Game->LOG(0, "Reverb process error=%d", status);
		}

		totalLength += currLength;
	}
}
#endif

