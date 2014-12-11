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
#include "ScStack.h"


IMPLEMENT_PERSISTENT(CScStack, false);

//////////////////////////////////////////////////////////////////////////
CScStack::CScStack(CBGame* inGame):CBBase(inGame)
{
	m_SP = -1;
}


//////////////////////////////////////////////////////////////////////////
CScStack::~CScStack()
{

#if _DEBUG
	//Game->LOG(0, "STAT: Stack size: %d, SP=%d", m_Values.GetSize(), m_SP);
#endif

	for(int i=0; i<m_Values.GetSize(); i++){
		delete m_Values[i];
	}
	m_Values.RemoveAll();
}


//////////////////////////////////////////////////////////////////////////
CScValue* CScStack::Pop()
{
	if(m_SP<0){
		Game->LOG(0, "Fatal: Stack underflow");
		return NULL;
	}

	return m_Values[m_SP--];
}


//////////////////////////////////////////////////////////////////////////
void CScStack::Push(CScValue* Val)
{
	m_SP++;
	
	if(m_SP < m_Values.GetSize()){
		m_Values[m_SP]->Cleanup();
		m_Values[m_SP]->Copy(Val);
	}
	else{
		CScValue* val = new CScValue(Game);
		val->Copy(Val);
		m_Values.Add(val);
	}
}


//////////////////////////////////////////////////////////////////////////
CScValue* CScStack::GetPushValue()
{
	m_SP++;
	
	if(m_SP >= m_Values.GetSize()){
		CScValue* val = new CScValue(Game);
		m_Values.Add(val);
	}
	m_Values[m_SP]->Cleanup();
	return m_Values[m_SP];
}



//////////////////////////////////////////////////////////////////////////
CScValue* CScStack::GetTop()
{
	if(m_SP<0 || m_SP >= m_Values.GetSize()) return NULL;
	else return m_Values[m_SP];
}


//////////////////////////////////////////////////////////////////////////
CScValue* CScStack::GetAt(int Index)
{
	Index = m_SP - Index;
	if(Index<0 || Index >= m_Values.GetSize()) return NULL;
	else return m_Values[Index];
}


//////////////////////////////////////////////////////////////////////////
void CScStack::CorrectParams(DWORD expected_params)
{
	int num_params = Pop()->GetInt();

	if(expected_params < num_params){ // too many params
		while(expected_params < num_params){
			//Pop();
			delete m_Values[m_SP - expected_params];
			m_Values.RemoveAt(m_SP - expected_params);
			num_params--;
			m_SP--;
		}
	}
	else if(expected_params > num_params){ // need more params
		while(expected_params > num_params){
			//Push(null_val);
			CScValue* null_val = new CScValue(Game);
			null_val->SetNULL();
			m_Values.InsertAt(m_SP - num_params+1, null_val);
			num_params++;
			m_SP++;

			if(m_Values.GetSize()>m_SP+1){
				delete m_Values[m_Values.GetSize()-1];
				m_Values.RemoveAt(m_Values.GetSize()-1);
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void CScStack::PushNULL()
{
	/*
	CScValue* val = new CScValue(Game);
	val->SetNULL();
	Push(val);
	delete val;
	*/
	GetPushValue()->SetNULL();
}


//////////////////////////////////////////////////////////////////////////
void CScStack::PushInt(int Val)
{
	/*
	CScValue* val = new CScValue(Game);
	val->SetInt(Val);
	Push(val);
	delete val;
	*/
	GetPushValue()->SetInt(Val);
}


//////////////////////////////////////////////////////////////////////////
void CScStack::PushFloat(double Val)
{
	/*
	CScValue* val = new CScValue(Game);
	val->SetFloat(Val);
	Push(val);
	delete val;
	*/
	GetPushValue()->SetFloat(Val);
}


//////////////////////////////////////////////////////////////////////////
void CScStack::PushBool(bool Val)
{
	/*
	CScValue* val = new CScValue(Game);
	val->SetBool(Val);
	Push(val);
	delete val;
	*/
	GetPushValue()->SetBool(Val);
}


//////////////////////////////////////////////////////////////////////////
void CScStack::PushString(const char *Val)
{
	/*
	CScValue* val = new CScValue(Game);
	val->SetString(Val);
	Push(val);
	delete val;
	*/
	GetPushValue()->SetString(Val);
}


//////////////////////////////////////////////////////////////////////////
void CScStack::PushNative(CBScriptable *Val, bool Persistent)
{
	/*
	CScValue* val = new CScValue(Game);
	val->SetNative(Val, Persistent);
	Push(val);
	delete val;
	*/

	GetPushValue()->SetNative(Val, Persistent);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CScStack::Persist(CBPersistMgr* PersistMgr){
	
	PersistMgr->Transfer(TMEMBER(Game));

	PersistMgr->Transfer(TMEMBER(m_SP));
	m_Values.Persist(PersistMgr);

	return S_OK;
}
