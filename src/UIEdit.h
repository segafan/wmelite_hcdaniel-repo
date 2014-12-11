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

#ifndef __WmeUIEdit_H__
#define __WmeUIEdit_H__


#include "UIObject.h"

class CUIEdit : public CUIObject  
{
public:
	DECLARE_PERSISTENT(CUIEdit, CUIObject);
	int m_MaxLength;
	int InsertChars(int Pos, BYTE* Chars, int Num);
	int DeleteChars(int Start, int End);
	bool m_CursorVisible;
	DWORD m_LastBlinkTime;
	virtual HRESULT Display(int OffsetX, int OffsetY);
	virtual bool HandleKeypress(SDL_Event* event);
	int m_ScrollOffset;
	int m_FrameWidth;
	DWORD m_CursorBlinkRate;
	void SetCursorChar(char* Char);
	char* m_CursorChar;
	int m_SelEnd;
	int m_SelStart;
	CBFont* m_FontSelected;
	CUIEdit(CBGame* inGame);
	virtual ~CUIEdit();

	HRESULT LoadFile(char* Filename);
	HRESULT LoadBuffer(BYTE* Buffer, bool Complete=true);
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent);

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();
};

#endif
