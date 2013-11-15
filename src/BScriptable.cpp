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
#include "BScriptable.h"


IMPLEMENT_PERSISTENT(CBScriptable, false);

//////////////////////////////////////////////////////////////////////////
CBScriptable::CBScriptable(CBGame* inGame, bool NoValue, bool Persistable):CBNamedObject(inGame)
{
	m_RefCount = 0;

	if(NoValue) m_ScValue = NULL;
	else m_ScValue = new CScValue(Game);

	m_Persistable = Persistable;

	m_ScProp = NULL;
}


//////////////////////////////////////////////////////////////////////////
CBScriptable::~CBScriptable()
{
	//if(m_RefCount>0) Game->LOG(0, "Warning: Destroying object, m_RefCount=%d", m_RefCount);
	SAFE_DELETE(m_ScValue);
	SAFE_DELETE(m_ScProp);
}


//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptable::ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name)
{
	/*
	Stack->CorrectParams(0);
	Stack->PushNULL();
	Script->RuntimeError("Call to undefined method '%s'.", Name);

	return S_OK;
	*/
	return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
CScValue* CBScriptable::ScGetProperty(char *Name)
{
	if(!m_ScProp) m_ScProp = new CScValue(Game);
	if(m_ScProp) return m_ScProp->GetProp(Name);
	else return NULL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptable::ScSetProperty(char *Name, CScValue *Value)
{
	if(!m_ScProp) m_ScProp = new CScValue(Game);
	if(m_ScProp) return m_ScProp->SetProp(Name, Value);
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
char* CBScriptable::ScToString()
{
	return "[native object]";
}

//////////////////////////////////////////////////////////////////////////
void* CBScriptable::ScToMemBuffer()
{
	return (void*)NULL;
}


//////////////////////////////////////////////////////////////////////////
int CBScriptable::ScToInt()
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
double CBScriptable::ScToFloat()
{
	return 0.0f;
}


//////////////////////////////////////////////////////////////////////////
bool CBScriptable::ScToBool()
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
void CBScriptable::ScSetString(const char* Val)
{
}


//////////////////////////////////////////////////////////////////////////
void CBScriptable::ScSetInt(int Val)
{
}


//////////////////////////////////////////////////////////////////////////
void CBScriptable::ScSetFloat(double Val)
{
}


//////////////////////////////////////////////////////////////////////////
void CBScriptable::ScSetBool(bool Val)
{
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBScriptable::Persist(CBPersistMgr *PersistMgr)
{
	PersistMgr->Transfer(TMEMBER(Game));
	PersistMgr->Transfer(TMEMBER(m_RefCount));
	PersistMgr->Transfer(TMEMBER(m_ScProp));
	PersistMgr->Transfer(TMEMBER(m_ScValue));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
int CBScriptable::ScCompare(CBScriptable *Val)
{
	if(this < Val) return -1;
	else if(this > Val) return 1;
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
void CBScriptable::ScDebuggerDesc(char* Buf, int BufSize)
{
	strcpy(Buf, ScToString());
}

//////////////////////////////////////////////////////////////////////////
bool CBScriptable::CanHandleMethod(char* EventMethod)
{
	return false;
}


//////////////////////////////////////////////////////////////////////////
CScScript* CBScriptable::InvokeMethodThread(char *MethodName)
{
	return NULL;
}


//////////////////////////////////////////////////////////////////////////
// IWmeDebugObject
//////////////////////////////////////////////////////////////////////////
const char* CBScriptable::DbgGetNativeClass()
{
	return GetClassName();
}

//////////////////////////////////////////////////////////////////////////
IWmeDebugProp* CBScriptable::DbgGetProperty(const char* Name)
{
	return ScGetProperty((char*)Name);
}
