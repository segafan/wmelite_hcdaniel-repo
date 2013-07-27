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

#ifndef __WmeScValue_H__
#define __WmeScValue_H__


#include "BBase.h"
#include "dcscript.h"	// Added by ClassView
#include <map>
#include <string>
#include "wme_debugger.h"

using namespace std;

class CScValue : public CBBase, public IWmeDebugProp
{
public:
	HRESULT DbgSendVariables(IWmeDebugClient* Client, EWmeDebuggerVariableType Type, CScScript* Script, unsigned int ScopeID);

	static int Compare(CScValue* Val1, CScValue* Val2);
	static int CompareStrict(CScValue* Val1, CScValue* Val2);
	TValType GetTypeTolerant();
	void Cleanup(bool IgnoreNatives = false);
	DECLARE_PERSISTENT(CScValue, CBBase);

	bool m_IsConstVar;
	HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent);
	void SetValue(CScValue* Val);
	bool m_Persistent;
	bool PropExists(char* Name);
	void Copy(CScValue* orig, bool CopyWhole = false);
	void SetStringVal(const char* Val);
	TValType GetType();
	bool GetBool(bool Default=false);
	int GetInt(int Default=0);
	double GetFloat(double Default=0.0f);
	char* GetString();
	void* GetMemBuffer();
	CBScriptable* GetNative();
	HRESULT DeleteProp(char *Name);
	void DeleteProps();
	void CleanProps(bool IncludingNatives);
	void SetBool(bool Val);
	void SetInt(int Val);
	void SetFloat(double Val);
	void SetString(const char* Val);
	void SetNULL();
	void SetNative(CBScriptable* Val, bool Persistent=false);
	void SetObject();
	void SetReference(CScValue* Val);
	bool IsNULL();
	bool IsNative();
	bool IsString();
	bool IsBool();
	bool IsFloat();
	bool IsInt();
	bool IsObject();
	HRESULT SetProp(char* Name, CScValue* Val, bool CopyWhole = false, bool SetAsConst=false);
	CScValue* GetProp(char* Name);
	CBScriptable* m_ValNative;
	CScValue* m_ValRef;
	bool m_ValBool;
	int m_ValInt;
	double m_ValFloat;
	char* m_ValString;
	TValType m_Type;
	CScValue(CBGame* inGame);
	CScValue(CBGame* inGame, bool Val);
	CScValue(CBGame* inGame, int Val);
	CScValue(CBGame* inGame, double Val);
	CScValue(CBGame* inGame, const char* Val);
	virtual ~CScValue();
	map<string, CScValue*> m_ValObject;
	map<string, CScValue*>::iterator m_ValIter;

	bool SetProperty(const char* PropName, int Value);
	bool SetProperty(const char* PropName, const char* Value);
	bool SetProperty(const char* PropName, double Value);
	bool SetProperty(const char* PropName, bool Value);
	bool SetProperty(const char* PropName);


// IWmeDebugProp interface implementation
public:
	virtual EWmeDebuggerPropType DbgGetType();

	// getters	
	virtual int DbgGetValInt();
	virtual double DbgGetValFloat();
	virtual bool DbgGetValBool();
	virtual const char* DbgGetValString();	
	virtual IWmeDebugObject* DbgGetValNative();	

	// setters
	virtual bool DbgSetVal(int Value);
	virtual bool DbgSetVal(double Value);
	virtual bool DbgSetVal(bool Value);
	virtual bool DbgSetVal(const char* Value);	
	virtual bool DbgSetVal();

	// properties
	virtual int DbgGetNumProperties();
	virtual bool DbgGetProperty(int Index, const char** Name, IWmeDebugProp** Value);

	virtual bool DbgGetDescription(char* Buf, int BufSize);
};


#endif
