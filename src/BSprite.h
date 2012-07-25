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

#ifndef __WmeBSprite_H__
#define __WmeBSprite_H__


#include "coll_templ.h"

class CBSprite: public CBScriptHolder
{
public:
	HRESULT KillAllSounds();
	CBSurface* GetSurface();
	char* m_EditorBgFile;
	int m_EditorBgOffsetX;
	int m_EditorBgOffsetY;
	int m_EditorBgAlpha;
	bool m_Streamed;
	bool m_StreamedKeepLoaded;
	void Cleanup();
	void SetDefaults();
	bool m_Precise;
	DECLARE_PERSISTENT(CBSprite, CBScriptHolder);

	bool m_EditorAllFrames;
	bool GetBoundingRect(LPRECT Rect, int X, int Y, float ScaleX=100, float ScaleY=100);
	int m_MoveY;
	int m_MoveX;
	HRESULT Display(int X, int Y, CBObject* Register=NULL, float ZoomX=100, float ZoomY=100, DWORD Alpha=0xFFFFFFFF, float Rotate=0.0f, TSpriteBlendMode BlendMode=BLEND_NORMAL);
	bool GetCurrentFrame(float ZoomX=100, float ZoomY=100);
	bool m_CanBreak;
	bool m_EditorMuted;
	bool m_Continuous;
	void Reset();
	CBObject* m_Owner;
	bool m_Changed;
	bool m_Paused;
	bool m_Finished;
	HRESULT LoadBuffer(BYTE* Buffer, bool Compete = true, int LifeTime=-1, TSpriteCacheType CacheType=CACHE_ALL);
	HRESULT LoadFile(char* Filename, int LifeTime=-1, TSpriteCacheType CacheType=CACHE_ALL);
	DWORD m_LastFrameTime;
	HRESULT Draw(int X, int Y, CBObject* Register = NULL, float ZoomX=100, float ZoomY=100, DWORD Alpha=0xFFFFFFFF);
	bool m_Looping;
	int m_CurrentFrame;
	HRESULT AddFrame(char* Filename, DWORD Delay=0, int HotspotX=0, int HotspotY=0, RECT* Rect=NULL);
	CBSprite(CBGame* inGame, CBObject* Owner=NULL);
	virtual ~CBSprite();
	CBArray<CBFrame*, CBFrame*> m_Frames;
	HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char* Name, CScValue* Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack* Stack, CScStack* ThisStack, char* Name);
	virtual char* ScToString();
};

#endif
