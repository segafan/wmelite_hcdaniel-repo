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

#ifndef __WmeBSubFrame_H__
#define __WmeBSubFrame_H__


#include "BBase.h"

class CBSubFrame : public CBScriptable
{
public:
	bool m_MirrorX;
	bool m_MirrorY;
	bool m_Decoration;
	HRESULT SetSurface (char* Filename, bool default_ck=true, BYTE ck_red=0, BYTE ck_green=0, BYTE ck_blue=0, int LifeTime=-1, bool KeepLoaded=false);
	HRESULT SetSurfaceSimple();
	DECLARE_PERSISTENT(CBSubFrame, CBScriptable);
	void SetDefaultRect();
	DWORD m_Transparent;
	HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent, bool Complete=true);
	bool m_EditorSelected;
	CBSubFrame(CBGame* inGame);
	virtual ~CBSubFrame();
	HRESULT LoadBuffer(BYTE* Buffer, int LifeTime, bool KeepLoaded);
	HRESULT Draw(int X, int Y, CBObject* Register = NULL, float ZoomX=100, float ZoomY=100, bool Precise = true, DWORD Alpha=0xFFFFFFFF, float Rotate=0.0f, TSpriteBlendMode BlendMode=BLEND_NORMAL);
	bool GetBoundingRect(LPRECT Rect, int X, int Y, float ScaleX=100, float ScaleY=100);

	int m_HotspotX;
	int m_HotspotY;
	DWORD m_Alpha;
	RECT m_Rect;	

	bool m_CKDefault;
	BYTE m_CKRed;
	BYTE m_CKGreen;
	BYTE m_CKBlue;
	int m_LifeTime;
	bool m_KeepLoaded;
	char* m_SurfaceFilename;

	bool m_2DOnly;
	bool m_3DOnly;

	CBSurface* m_Surface;

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char* Name, CScValue* Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack* Stack, CScStack* ThisStack, char* Name);
	virtual char* ScToString();

};

#endif
