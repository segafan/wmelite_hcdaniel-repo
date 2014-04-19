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

#include "dcgf.h"
#include "BKeyboardState.h"


IMPLEMENT_PERSISTENT(CBKeyboardState, false);

//////////////////////////////////////////////////////////////////////////
CBKeyboardState::CBKeyboardState(CBGame* inGame):CBScriptable(inGame)
{
	m_CurrentPrintable = false;
	m_CurrentCharCode = 0;
	m_CurrentKeyData = 0;

	m_CurrentShift = false;
	m_CurrentAlt = false;
	m_CurrentControl = false;
}


//////////////////////////////////////////////////////////////////////////
CBKeyboardState::~CBKeyboardState()
{

}


//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT CBKeyboardState::ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name)
{
	//////////////////////////////////////////////////////////////////////////
	// IsKeyDown
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "IsKeyDown")==0)
	{
		Stack->CorrectParams(1);
		CScValue* val = Stack->Pop();
		int vKey;

		if (val->m_Type==VAL_STRING && strlen(val->GetString())>0)
		{
			char* str = val->GetString();
			if(str[0]>='A' && str[0]<='Z') str[0] += ('a' - 'A');
			vKey = (int)str[0];
		}
		else vKey = val->GetInt();

    	const Uint8* state = SDL_GetKeyboardState(NULL);
		SDL_Scancode scanCode = SDL_GetScancodeFromKey(VKeyToKeyCode(vKey));
		bool isDown = state[scanCode] > 0;

		Stack->PushBool(isDown);
		return S_OK;
	}

	else return CBScriptable::ScCallMethod(Script, Stack, ThisStack, Name);
}


//////////////////////////////////////////////////////////////////////////
CScValue* CBKeyboardState::ScGetProperty(char *Name)
{
	m_ScValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "Type")==0){
		m_ScValue->SetString("keyboard");
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Key
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "Key")==0){
		if(m_CurrentPrintable){
			char key[2];
			key[0] = (char)m_CurrentCharCode;
			key[1] = '\0';
			m_ScValue->SetString(key);
		}
		else m_ScValue->SetString("");

		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Printable
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "Printable")==0){
		m_ScValue->SetBool(m_CurrentPrintable);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// KeyCode
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "KeyCode")==0){
		m_ScValue->SetInt(m_CurrentCharCode);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// IsShift
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "IsShift")==0){
		m_ScValue->SetBool(m_CurrentShift);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// IsAlt
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "IsAlt")==0){
		m_ScValue->SetBool(m_CurrentAlt);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// IsControl
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "IsControl")==0){
		m_ScValue->SetBool(m_CurrentControl);
		return m_ScValue;
	}

	else return CBScriptable::ScGetProperty(Name);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBKeyboardState::ScSetProperty(char *Name, CScValue *Value)
{
	/*
	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "Name")==0){
		SetName(Value->GetString());
		if(m_Renderer) SetWindowText(m_Renderer->m_Window, m_Name);
		return S_OK;
	}

	else*/ return CBScriptable::ScSetProperty(Name, Value);
}


//////////////////////////////////////////////////////////////////////////
char* CBKeyboardState::ScToString()
{
	return "[keyboard state]";
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBKeyboardState::ReadKey(SDL_Event* event)
{
	m_CurrentPrintable = (event->type == SDL_TEXTINPUT);
	/*
	 * Only save the code when it is a valid one.
	 *
	 * SDL seems to sometimes send 2 events upon keypress. One with the actual key, and
	 * one SDL_TEXTINPUT event later. Thus it can happen that the actual key is overwritten
	 * before the script that reads the key is actually executed.
	 * 
	 * The "fix" is to not overwrite the char code when the event is not actually a KEY_DOWN event.
	 */
	if (event->type == SDL_KEYDOWN)
	{
		m_CurrentCharCode = KeyCodeToVKey(event);
	}
	//m_CurrentKeyData = KeyData;

	m_CurrentControl = IsControlDown();
	m_CurrentAlt     = IsAltDown();
	m_CurrentShift   = IsShiftDown();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBKeyboardState::Persist(CBPersistMgr *PersistMgr)
{
	//if(!PersistMgr->m_Saving) Cleanup();
	CBScriptable::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_CurrentAlt));
	PersistMgr->Transfer(TMEMBER(m_CurrentCharCode));
	PersistMgr->Transfer(TMEMBER(m_CurrentControl));
	PersistMgr->Transfer(TMEMBER(m_CurrentKeyData));
	PersistMgr->Transfer(TMEMBER(m_CurrentPrintable));
	PersistMgr->Transfer(TMEMBER(m_CurrentShift));

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CBKeyboardState::IsShiftDown()
{
	int mod = SDL_GetModState();
	return (mod & KMOD_LSHIFT) || (mod & KMOD_RSHIFT);
}	

//////////////////////////////////////////////////////////////////////////
bool CBKeyboardState::IsControlDown()
{
	int mod = SDL_GetModState();
	return (mod & KMOD_LCTRL) || (mod & KMOD_RCTRL);
}

//////////////////////////////////////////////////////////////////////////
bool CBKeyboardState::IsAltDown()
{
	int mod = SDL_GetModState();
	return (mod & KMOD_LALT) || (mod & KMOD_RALT);
}

//////////////////////////////////////////////////////////////////////////
DWORD CBKeyboardState::KeyCodeToVKey(SDL_Event* event)
{
	if (event->type != SDL_KEYDOWN) return 0;

	switch (event->key.keysym.sym)
	{
	case SDLK_KP_ENTER:
		return SDLK_RETURN;
	default:
		return event->key.keysym.sym;
	}
}

//////////////////////////////////////////////////////////////////////////
SDL_Keycode CBKeyboardState::VKeyToKeyCode(DWORD vkey)
{
	// todo
	return (SDL_Keycode)vkey;
}
