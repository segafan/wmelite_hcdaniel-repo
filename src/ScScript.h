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

#ifndef __WmeScScript_H__
#define __WmeScScript_H__


#include "BBase.h"
#include "dcscript.h"	// Added by ClassView
#include "wme_debugger.h"

class CScEngine;
class CScScript : public CBBase, public IWmeDebugScript
{
public:
	HRESULT DbgSendScript(IWmeDebugClient* Client);
	HRESULT DbgSendVariables(IWmeDebugClient* Client);

	CBArray<int, int> m_Breakpoints;
	bool m_TracingMode;

	CScScript* m_ParentScript;
	bool m_Unbreakable;
	HRESULT FinishThreads();
	HRESULT CopyParameters(CScStack* Stack);

	void AfterLoad();

#ifdef __WIN32__
	static DWORD Call_cdecl(const void* args, size_t sz, DWORD func, bool* StackCorrupt);
	static DWORD Call_stdcall(const void* args, size_t sz, DWORD func, bool* StackCorrupt);
	static DWORD GetST0(void);
	static double GetST0Double(void);
#endif

	CScValue* m_Operand;
	CScValue* m_Reg1;
	bool m_Freezable;
	HRESULT Resume();
	HRESULT Pause();
	bool CanHandleEvent(char* EventName);
	bool CanHandleMethod(char* MethodName);
	HRESULT CreateThread(CScScript* Original, DWORD InitIP, const char* EventName);
	HRESULT CreateMethodThread(CScScript *Original, const char* MethodName);
	CScScript* InvokeEventHandler(const char* EventName, bool Unbreakable=false);
	DWORD m_TimeSlice;
	DECLARE_PERSISTENT(CScScript, CBBase);
	void RuntimeError(LPCSTR fmt, ...);
	HRESULT Run();
	HRESULT Finish(bool IncludingThreads = false);
	HRESULT Sleep(DWORD Duration);
	HRESULT WaitForExclusive(CBObject* Object);
	HRESULT WaitFor(CBObject* Object);
	DWORD m_WaitTime;
	bool m_WaitFrozen;
	CBObject* m_WaitObject;
	CScScript* m_WaitScript;
	TScriptState m_State;
	TScriptState m_OrigState;
	CScValue* GetVar(char* Name);
	DWORD GetFuncPos(const char* Name);
	DWORD GetEventPos(const char* Name);
	DWORD GetMethodPos(const char* Name);
	typedef struct{
		DWORD magic;
		DWORD version;
		DWORD code_start;
		DWORD func_table;
		DWORD symbol_table;
		DWORD event_table;
		DWORD externals_table;
		DWORD method_table;
	}TScriptHeader;


	typedef struct{
		char* name;
		DWORD pos;
	}TFunctionPos;

	typedef struct{
		char* name;
		DWORD pos;
	}TMethodPos;

	typedef struct{
		char* name;
		DWORD pos;
	}TEventPos;

	typedef struct{
		char* name;
		char* dll_name;
		TCallType call_type;
		TExternalType returns;
		int num_params;
		TExternalType* params;
	}TExternalFunction;


	CScStack* m_CallStack;
	CScStack* m_ThisStack;
	CScStack* m_ScopeStack;
	CScStack* m_Stack;
	CScValue* m_Globals;
	CScEngine* m_Engine;
	int m_CurrentLine;
	HRESULT ExecuteInstruction();
	char* GetString();
	DWORD GetDWORD();
	double GetFloat();
	void Cleanup();
	HRESULT Create(char* Filename, BYTE* Buffer, DWORD Size, CBScriptHolder* Owner);
	DWORD m_IP;
	DWORD m_BufferSize;
	BYTE* m_Buffer;
	CScScript(CBGame* inGame, CScEngine* Engine);
	virtual ~CScScript();
	char* m_Filename;
	char** m_Symbols;
	int m_NumSymbols;
	TFunctionPos* m_Functions;
	TMethodPos* m_Methods;
	TEventPos* m_Events;
	int m_NumExternals;
	TExternalFunction* m_Externals;
	int m_NumFunctions;
	int m_NumMethods;
	int m_NumEvents;
	bool m_Thread;
	bool m_MethodThread;
	char* m_ThreadEvent;
	CBScriptHolder* m_Owner;
	CScScript::TExternalFunction* GetExternal(char* Name);
	HRESULT ExternalCall(CScStack* Stack, CScStack* ThisStack, CScScript::TExternalFunction* Function);
private:
	HRESULT InitScript();
	HRESULT InitTables();


// IWmeDebugScript interface implementation
public:
	virtual int DbgGetLine();
	virtual const char* DbgGetFilename();
	virtual TScriptState DbgGetState();
	virtual int DbgGetNumBreakpoints();
	virtual int DbgGetBreakpoint(int Index);

	virtual bool DbgSetTracingMode(bool IsTracing);
	virtual bool DbgGetTracingMode();
};

#endif
