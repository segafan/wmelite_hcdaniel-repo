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

#ifndef __WmeBSoundMgr_H__
#define __WmeBSoundMgr_H__


#include "BBase.h"
#ifndef USE_SDL_MIXER
#include "bass.h"
#else
#include "SDL_mixer.h"
#endif

class CBSoundMgr : public CBBase  
{
public:	
	float PosToPan(int X, int Y);
	HRESULT ResumeAll();
	HRESULT PauseAll(bool IncludingMusic=true);
	HRESULT Cleanup();
	//DECLARE_PERSISTENT(CBSoundMgr, CBBase);
	BYTE GetMasterVolumePercent();
	HRESULT SetMasterVolumePercent(BYTE Percent);
	BYTE GetVolumePercent(TSoundType Type);
	HRESULT SetVolumePercent(TSoundType Type, BYTE Percent);
	HRESULT SetVolume(TSoundType Type, int Volume);
	DWORD m_VolumeOriginal;
	int m_VolumeMaster;
	int m_VolumeMusic;
	int m_VolumeSpeech;
	int m_VolumeSFX;
	HRESULT RemoveSound(CBSoundBuffer* Sound);
	CBSoundBuffer* AddSound(const char* Filename, TSoundType Type=SOUND_SFX, bool Streamed=false, DWORD initialPrivateVolume=100);
	HRESULT AddSound(CBSoundBuffer* Sound, TSoundType Type=SOUND_SFX);
	HRESULT InitLoop();
	HRESULT Initialize();
	bool m_SoundAvailable;
	CBSoundMgr(CBGame* inGame);
	virtual ~CBSoundMgr();
	CBArray<CBSoundBuffer*, CBSoundBuffer*> m_Sounds;
	void SaveSettings();
#ifdef USE_SDL_MIXER
	void InvalidateChannels(int channel_number);
	static void SDLMixer_channelDoneCallback(void *userdata, int channel_number);
#endif
};

#endif
