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
#include "AdNodeState.h"


IMPLEMENT_PERSISTENT(CAdNodeState, false);


//////////////////////////////////////////////////////////////////////////
CAdNodeState::CAdNodeState(CBGame* inGame):CBBase(inGame)
{
	m_Name = NULL;
	m_Active = false;
	for(int i=0; i<7; i++) m_Caption[i] = NULL;
	m_AlphaColor = 0;
	m_Filename = NULL;
	m_Cursor = NULL;
}


//////////////////////////////////////////////////////////////////////////
CAdNodeState::~CAdNodeState()
{
	SAFE_DELETE_ARRAY(m_Name);
	SAFE_DELETE_ARRAY(m_Filename);
	SAFE_DELETE_ARRAY(m_Cursor);
	for(int i=0; i<7; i++){
		SAFE_DELETE_ARRAY(m_Caption[i]);
	}
}


//////////////////////////////////////////////////////////////////////////
void CAdNodeState::SetName(char* Name)
{
	SAFE_DELETE_ARRAY(m_Name);
	CBUtils::SetString(&m_Name, Name);
}


//////////////////////////////////////////////////////////////////////////
void CAdNodeState::SetFilename(char* Filename)
{
	SAFE_DELETE_ARRAY(m_Filename);
	CBUtils::SetString(&m_Filename, Filename);
}


//////////////////////////////////////////////////////////////////////////
void CAdNodeState::SetCursor(char* Filename)
{
	SAFE_DELETE_ARRAY(m_Cursor);
	CBUtils::SetString(&m_Cursor, Filename);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdNodeState::Persist(CBPersistMgr *PersistMgr)
{	
	PersistMgr->Transfer(TMEMBER(Game));

	PersistMgr->Transfer(TMEMBER(m_Active));
	PersistMgr->Transfer(TMEMBER(m_Name));
	PersistMgr->Transfer(TMEMBER(m_Filename));
	PersistMgr->Transfer(TMEMBER(m_Cursor));
	PersistMgr->Transfer(TMEMBER(m_AlphaColor));
	for(int i=0; i<7; i++) PersistMgr->Transfer(TMEMBER(m_Caption[i]));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CAdNodeState::SetCaption(char *Caption, int Case)
{
	if(Case==0) Case = 1;
	if(Case<1 || Case>7) return;

	SAFE_DELETE_ARRAY(m_Caption[Case-1]);
	m_Caption[Case-1] = new char[strlen(Caption)+1];
	if(m_Caption[Case-1]){
		strcpy(m_Caption[Case-1], Caption);
		Game->m_StringTable->Expand(&m_Caption[Case-1]);
	}
}


//////////////////////////////////////////////////////////////////////////
char* CAdNodeState::GetCaption(int Case)
{
	if(Case==0) Case = 1;
	if(Case<1 || Case>7 || m_Caption[Case-1]==NULL) return "";
	else return m_Caption[Case-1];
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdNodeState::TransferEntity(CAdEntity* Entity, bool IncludingSprites, bool Saving)
{
	if(!Entity) return E_FAIL;

	// hack!
	if(this->Game != Entity->Game) this->Game = Entity->Game;

	if(Saving){
		for(int i=0; i<7; i++){
			if(Entity->m_Caption[i]) SetCaption(Entity->m_Caption[i], i);
		}
		if(!Entity->m_Region && Entity->m_Sprite && Entity->m_Sprite->m_Filename){
			if(IncludingSprites) SetFilename(Entity->m_Sprite->m_Filename);
			else SetFilename("");
		}
		if(Entity->m_Cursor && Entity->m_Cursor->m_Filename) SetCursor(Entity->m_Cursor->m_Filename);
		m_AlphaColor = Entity->m_AlphaColor;
		m_Active = Entity->m_Active;
	}
	else{
		for(int i=0; i<7; i++){
			if(m_Caption[i]) Entity->SetCaption(m_Caption[i], i);
		}
		if(m_Filename && !Entity->m_Region && IncludingSprites && strcmp(m_Filename, "")!=0){
			if(!Entity->m_Sprite || !Entity->m_Sprite->m_Filename || CBPlatform::stricmp(Entity->m_Sprite->m_Filename, m_Filename)!=0)
				Entity->SetSprite(m_Filename);
		}
		if(m_Cursor){
			if(!Entity->m_Cursor || !Entity->m_Cursor->m_Filename || CBPlatform::stricmp(Entity->m_Cursor->m_Filename, m_Cursor)!=0)
				Entity->SetCursor(m_Cursor);
		}

		Entity->m_Active = m_Active;
		Entity->m_AlphaColor = m_AlphaColor;
	}

	return S_OK;
}
