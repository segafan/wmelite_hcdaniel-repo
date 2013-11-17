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

#ifndef __WmeAdActor_H__
#define __WmeAdActor_H__


#include "dctypes.h"	// Added by ClassView
#include "AdTypes.h"	// Added by ClassView
#include "BPoint.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAdActor : public CAdTalkHolder
{
public:	
	TDirection AngleToDirection(int Angle);
	DECLARE_PERSISTENT(CAdActor, CAdTalkHolder);	
	virtual int GetHeight();
	CBSprite* GetTalkStance(char* Stance);
	virtual void GoTo(int X, int Y, TDirection AfterWalkDir=DI_NONE);
	CBPoint* m_TargetPoint;
	virtual HRESULT Update();
	virtual HRESULT Display();
	TDirection m_TargetDir;
	TDirection m_AfterWalkDir;
	virtual void TurnTo(TDirection dir);
	CAdPath* m_Path;
	CAdSpriteSet* m_WalkSprite;
	CAdSpriteSet* m_StandSprite;
	CAdSpriteSet* m_TurnLeftSprite;
	CAdSpriteSet* m_TurnRightSprite;
	CBArray<CAdSpriteSet*, CAdSpriteSet*> m_TalkSprites;
	CBArray<CAdSpriteSet*, CAdSpriteSet*> m_TalkSpritesEx;
	TDirection m_Dir;
	CAdActor(CBGame* inGame/*=NULL*/);
	virtual ~CAdActor();
	HRESULT LoadFile(char* Filename);
	HRESULT LoadBuffer(BYTE* Buffer, bool Complete=true);

	// new anim system
	char* m_TalkAnimName;
	char* m_IdleAnimName;
	char* m_WalkAnimName;
	char* m_TurnLeftAnimName;
	char* m_TurnRightAnimName;
	CBArray<CAdSpriteSet*, CAdSpriteSet*> m_Anims;
	virtual HRESULT PlayAnim(char* Filename);
	CAdSpriteSet* GetAnimByName(char* AnimName);

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();

private:
	HRESULT SetDefaultAnimNames();	
	CBSprite* GetTalkStanceOld(char *Stance);
	HRESULT MergeAnims(char* AnimsFilename);
	CBSprite* m_AnimSprite2;

	void InitLine(CBPoint StartPt, CBPoint EndPt);
	void GetNextStep();
	void FollowPath();				
	double m_PFStepX;
	double m_PFStepY;
	double m_PFX;
	double m_PFY;
	int m_PFCount;
};

#endif // __WmeAdActor_H__
