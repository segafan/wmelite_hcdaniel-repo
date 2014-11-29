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

#ifndef __WmeBFrame_H__
#define __WmeBFrame_H__


class CBFrame: public CBScriptable
{
public:
	bool m_KillSound;
	bool m_Keyframe;
	HRESULT OneTimeDisplay(CBObject* Owner, bool Muted=false);
	DECLARE_PERSISTENT(CBFrame, CBScriptable);
	CBSound* m_Sound;
	bool m_EditorExpanded;
	bool GetBoundingRect(LPRECT Rect, int X, int Y, float ScaleX=100, float ScaleY=100);
	HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);
	int m_MoveY;
	int m_MoveX;
	DWORD m_Delay;
	CBArray<CBSubFrame*, CBSubFrame*> m_Subframes;
	HRESULT Draw(int X, int Y, CBObject* Register = NULL, float ZoomX=100, float ZoomY=100, bool Precise = true, DWORD Alpha=0xFFFFFFFF, bool AllFrames=false, float Rotate=0.0f, TSpriteBlendMode BlendMode=BLEND_NORMAL);
	HRESULT LoadBuffer(BYTE* Buffer, int LifeTime, bool KeepLoaded);

	CBFrame(CBGame* inGame);
	virtual ~CBFrame();

	CBArray<char*, char*> m_ApplyEvent;

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char* Name, CScValue* Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack* Stack, CScStack* ThisStack, char* Name);
	virtual char* ScToString();

};

#endif
