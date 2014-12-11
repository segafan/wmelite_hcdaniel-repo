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

#ifndef __WmeUIObject_H__
#define __WmeUIObject_H__


#include "BObject.h"
#include "dctypes.h"	// Added by ClassView

class CUITiledImage;

class CUIObject : public CBObject
{
public:

	HRESULT GetTotalOffset(int* OffsetX, int* OffsetY);
	bool m_CanFocus;
	HRESULT Focus();
	virtual HRESULT HandleMouse(TMouseEvent Event, TMouseButton Button);	
	bool IsFocused();
	bool m_ParentNotify;
	DECLARE_PERSISTENT(CUIObject, CBObject);
	CUIObject* m_Parent;
	virtual HRESULT Display(int OffsetX=0, int OffsetY=0);
	virtual void CorrectSize();
	bool m_SharedFonts;
	bool m_SharedImages;
	void SetText(const char* Text);
	char* m_Text;
	CBFont* m_Font;
	bool m_Visible;
	CUITiledImage* m_Back;
	bool m_Disable;
	CUIObject(CBGame* inGame=NULL);
	virtual ~CUIObject();
	int m_Width;
	int m_Height;
	TUIObjectType m_Type;
	CBSprite* m_Image;
	void SetListener(CBScriptHolder* Object, CBScriptHolder* ListenerObject, DWORD ListenerParam);
	CBScriptHolder* m_ListenerParamObject;
	DWORD m_ListenerParamDWORD;
	CBScriptHolder* m_ListenerObject;
	CUIObject* m_FocusedWidget;
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent);

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();
};

#endif
