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

#ifndef __WmeBObject_H__
#define __WmeBObject_H__


#include "SDL.h"


class CBSprite;
class CBSound;
class CBSurface;


class CBObject : public CBScriptHolder
{
public:
	TSpriteBlendMode m_BlendMode;
	virtual HRESULT AfterMove();
	float m_RelativeRotate;
	bool m_RotateValid;
	float m_Rotate;
	void SetSoundEvent(char* EventName);
	bool m_Rotatable;
	DWORD m_AlphaColor;
	float m_Scale;
	float m_ScaleX;
	float m_ScaleY;
	float m_RelativeScale;
	virtual bool IsReady();
	virtual bool GetExtendedFlag(char* FlagName);
	virtual HRESULT ResetSoundPan();
	virtual HRESULT UpdateSounds();
	HRESULT UpdateOneSound(CBSound* Sound);
	bool m_AutoSoundPanning;
	DWORD m_SFXStart;
	int m_SFXVolume;
	HRESULT SetSFXTime(DWORD Time);
	HRESULT SetSFXVolume(int Volume);
	HRESULT ResumeSFX();
	HRESULT PauseSFX();
	HRESULT StopSFX(bool DeleteSound=true);
	HRESULT PlaySFX(char* Filename, bool Looping=false, bool PlayNow=true, char* EventName=NULL, DWORD LoopStart=0);
	CBSound* m_SFX;

	TSFXType m_SFXType;
	float m_SFXParam1;
	float m_SFXParam2;
	float m_SFXParam3;
	float m_SFXParam4;

	virtual bool HandleMouseWheel(int Delta);
	virtual HRESULT HandleMouse(TMouseEvent Event, TMouseButton Button);
	virtual bool HandleKeypress(SDL_Event* event);
	virtual int GetHeight();
	HRESULT SetCursor(char* Filename);
	HRESULT SetActiveCursor(char* Filename);
	HRESULT Cleanup();
	char* GetCaption(int Case=1);
	void SetCaption(char* Caption, int Case=1);
	bool m_EditorSelected;
	bool m_EditorAlwaysRegister;
	bool m_EditorOnly;
	bool m_Is3D;
	DECLARE_PERSISTENT(CBObject, CBScriptHolder);
	virtual HRESULT ShowCursor();
	CBSprite* m_Cursor;
	bool m_SharedCursors;
	CBSprite* m_ActiveCursor;
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent);
	virtual HRESULT Listen(CBScriptHolder* param1, DWORD param2);
	bool m_Ready;
	bool m_Registrable;
	bool m_Zoomable;
	bool m_Shadowable;
	RECT m_Rect;
	bool m_RectSet;
	int m_ID;
	bool m_Movable;
	CBObject(CBGame* inGame);
	virtual ~CBObject();
	char* m_Caption[7];
	char* m_SoundEvent;
	int m_PosY;
	int m_PosX;
	bool m_SaveState;

	// base
	virtual HRESULT Update()  { return E_FAIL; };
	virtual HRESULT Display() { return E_FAIL; };
	virtual HRESULT InvalidateDeviceObjects()  { return S_OK; };
	virtual HRESULT RestoreDeviceObjects()     { return S_OK; };
	bool m_NonIntMouseEvents;


public:
	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();
};

#endif
