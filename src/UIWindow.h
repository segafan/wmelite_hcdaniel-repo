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

#ifndef __WmeUIWindow_H__
#define __WmeUIWindow_H__


#include "UIObject.h"

class CUIWindow : public CUIObject  
{
public:
	HRESULT GetWindowObjects(CBArray<CUIObject*, CUIObject*>& Objects, bool InteractiveOnly);

	bool m_PauseMusic;
	void Cleanup();
	virtual void MakeFreezable(bool Freezable);
	CBViewport* m_Viewport;
	bool m_ClipContents;
	bool m_InGame;
	bool m_IsMenu;
	bool m_FadeBackground;
	DWORD m_FadeColor;
	virtual bool HandleMouseWheel(int Delta);
	CUIWindow* m_ShieldWindow;
	CUIButton* m_ShieldButton;
	HRESULT Close();
	HRESULT GoSystemExclusive();
	HRESULT GoExclusive();
	TWindowMode m_Mode;
	HRESULT MoveFocus(bool Forward=true);
	virtual HRESULT HandleMouse(TMouseEvent Event, TMouseButton Button);
	POINT m_DragFrom;
	bool m_Dragging;
	DECLARE_PERSISTENT(CUIWindow, CUIObject);
	bool m_Transparent;
	HRESULT ShowWidget(char* Name, bool Visible=true);
	HRESULT EnableWidget(char* Name, bool Enable=true);
	RECT m_TitleRect;
	RECT m_DragRect;
	virtual HRESULT Display(int OffsetX=0, int OffsetY=0);
	CUIWindow(CBGame* inGame);
	virtual ~CUIWindow();
	virtual bool HandleKeypress(SDL_Event* event);
	CBArray<CUIObject*, CUIObject*> m_Widgets;
	TTextAlign m_TitleAlign;
	HRESULT LoadFile(char* Filename);
	HRESULT LoadBuffer(BYTE* Buffer, bool Complete=true);
	CUITiledImage* m_BackInactive;
	CBFont* m_FontInactive;
	CBSprite* m_ImageInactive;
	virtual HRESULT Listen(CBScriptHolder* param1, DWORD param2);
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent);

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();
};

#endif
