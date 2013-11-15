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

#ifndef __WmeBKeyboardState_H__
#define __WmeBKeyboardState_H__


#include "BBase.h"
#include "SDL.h"

class CBKeyboardState : public CBScriptable
{
public:
	DWORD m_CurrentKeyData;
	DWORD m_CurrentCharCode;
	bool m_CurrentPrintable;

	bool m_CurrentShift;
	bool m_CurrentAlt;
	bool m_CurrentControl;

	DECLARE_PERSISTENT(CBKeyboardState, CBScriptable);
	CBKeyboardState(CBGame* inGame);
	virtual ~CBKeyboardState();
	HRESULT ReadKey(SDL_Event* event);

	static bool IsShiftDown();
	static bool IsControlDown();
	static bool IsAltDown();

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();

private:
	DWORD KeyCodeToVKey(SDL_Event* event);
	SDL_Keycode VKeyToKeyCode(DWORD vkey);
};

#endif
