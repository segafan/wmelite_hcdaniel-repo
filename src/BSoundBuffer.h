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

#ifndef __WmeBSoundBuffer_H__
#define __WmeBSoundBuffer_H__


#include "BBase.h"
#ifndef USE_SDL_MIXER
#include "bass.h"
#ifdef  USE_BASS_FX
#include "bass_fx.h"
#endif
#else
#include "SDL_mixer.h"
#endif

#ifdef USE_LIBEFFECTS_REVERB
#include "Wrapper/ReverbLibrary.h"
#endif

class CBSoundBuffer : public CBBase  
{
public:

	CBSoundBuffer(CBGame* inGame);
	virtual ~CBSoundBuffer();


	HRESULT Pause();
	HRESULT Play(bool Looping=false, DWORD StartSample=0);
	HRESULT Resume();
	HRESULT Stop();
	bool IsPlaying();

	void SetLooping(bool looping);

	DWORD GetPosition();
	HRESULT SetPosition(DWORD Pos);	
	DWORD GetLength();

	HRESULT SetLoopStart(DWORD Pos);
	DWORD GetLoopStart() const { return m_LoopStart; }

	HRESULT SetPan(float Pan);
	HRESULT SetPrivateVolume(int Volume);	
	HRESULT SetVolume(int Volume);

	void SetType(TSoundType Type);

	HRESULT LoadFromFile(const char* Filename, bool ForceReload=false);
	void SetStreaming(bool Streamed, DWORD NumBlocks=0, DWORD BlockSize=0);
	HRESULT ApplyFX(TSFXType Type, float Param1, float Param2, float Param3, float Param4);

	bool m_FreezePaused;
	DWORD m_LoopStart;
	TSoundType m_Type;
	bool m_Looping;
	CBFile* m_File;
	char* m_Filename;
	bool m_Streamed;
	int m_PrivateVolume;

#ifndef USE_SDL_MIXER
	
	HSTREAM m_Stream;
	HSYNC m_Sync;

#ifdef USE_BASS_FX

	HFX m_EffectHandle;

#endif

#ifdef USE_LIBEFFECTS_REVERB

	ReverbContext m_context;
	HDSP m_BASS_DSP_handle;

	static void CALLBACK DSPProc(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user);


#endif

	static void CALLBACK LoopSyncProc(HSYNC handle, DWORD channel, DWORD data, void* user);

	static void CALLBACK FileCloseProc(void* user);
	static QWORD CALLBACK FileLenProc(void* user);
	static DWORD CALLBACK FileReadProc(void *buffer, DWORD length, void* user);
	static BOOL CALLBACK FileSeekProc(QWORD offset, void *user);

#else

	Mix_Chunk *m_chunk;
	Mix_Music *m_music;
	SDL_RWops *m_rwops;
	int m_currChannel;

	void InvalidateChannelNumber(int finished_channel);

	static Sint64 FileSizeImpl(SDL_RWops *ops);
	static Sint64 FileSeekImpl(SDL_RWops *ops, Sint64 offset, int whence);
	static size_t FileReadImpl(SDL_RWops *ops, void *buffer, size_t size, size_t nmemb);
	static size_t FileWriteImpl(SDL_RWops *ops, const void *buffer, size_t size, size_t nmemb);
	static int FileCloseImpl(SDL_RWops *ops);

#endif

};

#endif
