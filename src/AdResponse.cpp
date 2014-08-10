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
#include "AdResponse.h"


IMPLEMENT_PERSISTENT(CAdResponse, false);

//////////////////////////////////////////////////////////////////////////
CAdResponse::CAdResponse(CBGame* inGame):CBObject(inGame)
{
	m_Text = NULL;
	m_TextOrig = NULL;
	m_Icon = m_IconHover = m_IconPressed = NULL;
	m_Font = NULL;
	m_ID = 0;
	m_ResponseType = RESPONSE_ALWAYS;
}


//////////////////////////////////////////////////////////////////////////
CAdResponse::~CAdResponse()
{
	SAFE_DELETE_ARRAY(m_Text);
	SAFE_DELETE_ARRAY(m_TextOrig);
	SAFE_DELETE(m_Icon);
	SAFE_DELETE(m_IconHover);
	SAFE_DELETE(m_IconPressed);
	if(m_Font) Game->m_FontStorage->RemoveFont(m_Font);
}


//////////////////////////////////////////////////////////////////////////
void CAdResponse::SetText(char* Text)
{
	CBUtils::SetString(&m_Text, Text);
	CBUtils::SetString(&m_TextOrig, Text);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdResponse::SetIcon(char* Filename)
{
	SAFE_DELETE(m_Icon);
	m_Icon = new CBSprite(Game);
	if(!m_Icon || FAILED(m_Icon->LoadFile(Filename)))
	{
		Game->LOG(0, "CAdResponse::SetIcon failed for file '%s'", Filename);
		SAFE_DELETE(m_Icon);
		return E_FAIL;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CAdResponse::SetFont(char* Filename)
{
	if(m_Font) Game->m_FontStorage->RemoveFont(m_Font);
	m_Font = Game->m_FontStorage->AddFont(Filename);
	if(!m_Font)
	{
		Game->LOG(0, "CAdResponse::SetFont failed for file '%s'", Filename);
		return E_FAIL;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CAdResponse::SetIconHover(char* Filename)
{
	SAFE_DELETE(m_IconHover);
	m_IconHover = new CBSprite(Game);
	if(!m_IconHover || FAILED(m_IconHover->LoadFile(Filename))){
		Game->LOG(0, "CAdResponse::SetIconHover failed for file '%s'", Filename);
		SAFE_DELETE(m_IconHover);
		return E_FAIL;
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdResponse::SetIconPressed(char *Filename)
{
	SAFE_DELETE(m_IconPressed);
	m_IconPressed = new CBSprite(Game);
	if(!m_IconPressed || FAILED(m_IconPressed->LoadFile(Filename))){
		Game->LOG(0, "CAdResponse::SetIconPressed failed for file '%s'", Filename);
		SAFE_DELETE(m_IconPressed);
		return E_FAIL;
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdResponse::Persist(CBPersistMgr* PersistMgr){

	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_Icon));
	PersistMgr->Transfer(TMEMBER(m_IconHover));
	PersistMgr->Transfer(TMEMBER(m_IconPressed));
	PersistMgr->Transfer(TMEMBER(m_ID));
	PersistMgr->Transfer(TMEMBER(m_Text));
	PersistMgr->Transfer(TMEMBER(m_TextOrig));
	PersistMgr->Transfer(TMEMBER_INT(m_ResponseType));
	PersistMgr->Transfer(TMEMBER(m_Font));

	return S_OK;
}
