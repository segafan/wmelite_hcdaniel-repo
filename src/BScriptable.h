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

#ifndef __WmeBScriptable_H__
#define __WmeBScriptable_H__


#include "BNamedObject.h"
#include "wme_debugger.h"

class CScValue;
class CScStack;
class CScScript;

class CBScriptable : public CBNamedObject, public IWmeDebugObject
{
public:
	virtual CScScript* InvokeMethodThread(char* MethodName);
	DECLARE_PERSISTENT(CBScriptable, CBNamedObject);

	CBScriptable(CBGame* inGame, bool NoValue = false, bool Persistable=true);
	virtual ~CBScriptable();

	// high level scripting interface
	virtual bool CanHandleMethod(char* EventMethod);
	virtual HRESULT ScSetProperty(char* Name, CScValue* Value);
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack* Stack, CScStack* ThisStack, char* Name);
	virtual char* ScToString();
	virtual void* ScToMemBuffer();
	virtual int ScToInt();
	virtual double ScToFloat();
	virtual bool ScToBool();
	virtual void ScSetString(const char* Val);
	virtual void ScSetInt(int Val);
	virtual void ScSetFloat(double Val);
	virtual void ScSetBool(bool Val);
	virtual int ScCompare(CBScriptable* Val);
	virtual void ScDebuggerDesc(char* Buf, int BufSize);
	int m_RefCount;
	CScValue* m_ScValue;
	CScValue* m_ScProp;

public:
	// IWmeDebugObject
	const char* DbgGetNativeClass();
	IWmeDebugProp* DbgGetProperty(const char* Name);

};

#endif
