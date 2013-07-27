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

#ifndef __WmeBScriptHolder_H__
#define __WmeBScriptHolder_H__


class CBScriptHolder : public CBScriptable
{
public:
	DECLARE_PERSISTENT(CBScriptHolder, CBScriptable);

	CBScriptHolder(CBGame* inGame);
	virtual ~CBScriptHolder();

	virtual CScScript* InvokeMethodThread(char* MethodName);
	virtual void MakeFreezable(bool Freezable);
	bool CanHandleEvent(char* EventName);
	virtual bool CanHandleMethod(char* EventMethod);
	HRESULT Cleanup();
	HRESULT RemoveScript(CScScript* Script);
	HRESULT AddScript(char* Filename);
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent);	
	virtual HRESULT Listen(CBScriptHolder* param1, DWORD param2);	
	HRESULT ApplyEvent(const char* EventName, bool Unbreakable=false);
	void SetFilename(char* Filename);
	HRESULT ParseProperty(BYTE* Buffer, bool Complete=true);

	char* m_Filename;
	bool m_Freezable;
	bool m_Ready;
	CBArray<CScScript*, CScScript*> m_Scripts;

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();
	virtual void ScDebuggerDesc(char* Buf, int BufSize);

	// IWmeObject
public:
	virtual bool SendEvent(const char* EventName);
};

#endif
