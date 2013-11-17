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

#ifndef __WmeBSound_H__
#define __WmeBSound_H__


#include "BBase.h"
#include "dctypes.h"	// Added by ClassView

class CBSound : public CBBase  
{
public:
	HRESULT SetPan(float Pan);
	int m_SoundPrivateVolume;
	int GetVolume();
	HRESULT SetVolume(int Volume);
	HRESULT SetPrivateVolume(int Volume);
	HRESULT SetLoopStart(DWORD Pos);
	DWORD GetPositionTime();
	HRESULT SetPositionTime(DWORD Time);
	bool m_SoundPaused;
	bool m_SoundFreezePaused;
	bool IsPlaying();
	bool IsPaused();
	bool m_SoundPlaying;
	bool m_SoundLooping;
	DWORD m_SoundLoopStart;
	DWORD m_SoundPosition;
	DECLARE_PERSISTENT(CBSound, CBBase);
	HRESULT Resume();
	HRESULT Pause(bool FreezePaused = false);
	HRESULT Stop();
	HRESULT Play(bool Looping=false);
	DWORD GetLength();
	bool m_SoundStreamed;
	TSoundType m_SoundType;
	char* m_SoundFilename;
	HRESULT SetSoundSimple();
	HRESULT SetSound(char* Filename, TSoundType Type=SOUND_SFX, bool Streamed=false, DWORD initialPrivateVolume=100);
	CBSound(CBGame* inGame);
	virtual ~CBSound();

	HRESULT ApplyFX(TSFXType Type=SFX_NONE, float Param1=0, float Param2=0, float Param3=0, float Param4=0);

	TSFXType m_SFXType;
	float m_SFXParam1;
	float m_SFXParam2;
	float m_SFXParam3;
	float m_SFXParam4;

private:
	CBSoundBuffer* m_Sound;

};

#endif
