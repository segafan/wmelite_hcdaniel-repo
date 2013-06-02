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

#ifndef __WmeVidTheoraPlayer_H__
#define __WmeVidTheoraPlayer_H__

#include "BBase.h"

class CBSurfaceSDL;
class TheoraVideoClip;

class CVidTheoraPlayer : public CBBase
{
public:	
	DECLARE_PERSISTENT(CVidTheoraPlayer, CBBase);

	CVidTheoraPlayer(CBGame* inGame);
	virtual ~CVidTheoraPlayer();

	enum { THEORA_STATE_NONE=0, THEORA_STATE_PLAYING=1, THEORA_STATE_PAUSED=2, THEORA_STATE_FINISHED=3 };


	HRESULT Initialize(char* filename, char* subtitleFile = NULL);
	HRESULT InitializeSimple();
	HRESULT Update();
	HRESULT Play(TVideoPlayback type=VID_PLAY_CENTER, int x=0, int y=0, bool freezeGame=false, bool freezeMusic=true, bool looping=false, DWORD startTime=0, float forceZoom=-1.0f, int volume=-1);
	HRESULT Stop();
	HRESULT Display(DWORD alpha = 0xFFFFFFFF);

	HRESULT Pause();
	HRESULT Resume();

	bool IsPlaying()  { return m_State==THEORA_STATE_PLAYING;  };
	bool IsFinished() { return m_State==THEORA_STATE_FINISHED; };
	bool IsPaused()   { return m_State==THEORA_STATE_PAUSED;   };

	HRESULT SetAlphaImage(char* filename);

	void SetFreezePaused(bool val) { m_FreezePaused = val; }
	bool IsFreezePaused() const { return m_FreezePaused; }

	int m_PosX;
	int m_PosY;

private:
	void SetDefaults();
	void Cleanup();
	HRESULT SeekToTime(DWORD time);
	float GetMovieTime();

	bool m_FreezePaused;

	CBFile* m_File;
	char* m_Filename;
	CBSurfaceSDL* m_Texture;	
	int m_State;
	TVideoPlayback m_PlaybackType;
	float m_PlayZoom;
	int m_Volume;

	bool m_Looping;
	bool m_FreezeGame;
	DWORD m_CurrentTime;

	char* m_AlphaFilename;
	CBSurfaceSDL* m_AlphaTexture;

	int m_SavedState;
	DWORD m_SavedPos;

	TheoraVideoClip* m_Clip;
};

#endif // __WmeVidTheoraPlayer_H__
