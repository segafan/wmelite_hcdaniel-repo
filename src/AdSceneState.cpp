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
#include "AdSceneState.h"


IMPLEMENT_PERSISTENT(CAdSceneState, false);

//////////////////////////////////////////////////////////////////////////
CAdSceneState::CAdSceneState(CBGame* inGame):CBBase(inGame)
{
	m_Filename = NULL;
}


//////////////////////////////////////////////////////////////////////////
CAdSceneState::~CAdSceneState()
{
	SAFE_DELETE_ARRAY(m_Filename);

	for(int i=0; i<m_NodeStates.GetSize(); i++)	delete m_NodeStates[i];
	m_NodeStates.RemoveAll();
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSceneState::Persist(CBPersistMgr *PersistMgr)
{
	PersistMgr->Transfer(TMEMBER(m_Filename));
	m_NodeStates.Persist(PersistMgr);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CAdSceneState::SetFilename(char *Filename)
{
	SAFE_DELETE_ARRAY(m_Filename);
	m_Filename = new char [strlen(Filename)+1];
	if(m_Filename) strcpy(m_Filename, Filename);
}


//////////////////////////////////////////////////////////////////////////
CAdNodeState* CAdSceneState::GetNodeState(char *Name, bool Saving)
{
	for(int i=0; i<m_NodeStates.GetSize(); i++){
		if(CBPlatform::stricmp(m_NodeStates[i]->m_Name, Name)==0) return m_NodeStates[i];
	}

	if(Saving){
		CAdNodeState* ret = new CAdNodeState(Game);
		ret->SetName(Name);
		m_NodeStates.Add(ret);

		return ret;
	}
	else return NULL;
}
