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

#ifndef __WmeAdSentence_H__
#define __WmeAdSentence_H__


#include "BBase.h"
#include "dctypes.h"	// Added by ClassView

class CAdSentence : public CBBase  
{
public:	
	bool m_Freezable;
	bool m_FixedPos;
	CBSprite* m_CurrentSprite;
	char* m_CurrentSkelAnim;
	HRESULT Update(TDirection Dir=DI_DOWN);
	HRESULT SetupTalkFile(char* SoundFilename);
	DECLARE_PERSISTENT(CAdSentence, CBBase);
	HRESULT Finish();
	void SetSound(CBSound* Sound);
	bool m_SoundStarted;
	CBSound* m_Sound;
	TTextAlign m_Align;
	HRESULT Display();
	int m_Width;
	POINT m_Pos;
	CBFont* m_Font;
	char* GetNextStance();
	char* GetCurrentStance();
	void SetStances(char* Stances);
	void SetText(char* Text);
	int m_CurrentStance;
	DWORD m_StartTime;
	char* m_Stances;
	char* m_Text;	
	DWORD m_Duration;
	CAdSentence(CBGame* inGame);
	virtual ~CAdSentence();
	CAdTalkDef* m_TalkDef;

	bool CanSkip();

private:
	char* m_TempStance;
	char* GetStance(int Stance);
	
};

#endif
