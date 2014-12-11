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
#include "AdTalkDef.h"


IMPLEMENT_PERSISTENT(CAdTalkDef, false);

//////////////////////////////////////////////////////////////////////////
CAdTalkDef::CAdTalkDef(CBGame* inGame):CBObject(inGame)
{
	m_DefaultSpriteFilename = NULL;
	m_DefaultSprite = NULL;

	m_DefaultSpriteSetFilename = NULL;
	m_DefaultSpriteSet = NULL;
}


//////////////////////////////////////////////////////////////////////////
CAdTalkDef::~CAdTalkDef()
{
	for(int i=0; i<m_Nodes.GetSize(); i++) delete m_Nodes[i];
	m_Nodes.RemoveAll();

	SAFE_DELETE_ARRAY(m_DefaultSpriteFilename);
	SAFE_DELETE(m_DefaultSprite);

	SAFE_DELETE_ARRAY(m_DefaultSpriteSetFilename);
	SAFE_DELETE(m_DefaultSpriteSet);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdTalkDef::LoadFile(const char* Filename)
{
	BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename);
	if(Buffer==NULL){
		Game->LOG(0, "CAdTalkDef::LoadFile failed for file '%s'", Filename);
		return E_FAIL;
	}

	HRESULT ret;

	CBUtils::SetString(&m_Filename, Filename);

	if(FAILED(ret = LoadBuffer(Buffer, true))) Game->LOG(0, "Error parsing TALK file '%s'", Filename);

	delete [] Buffer;

	return ret;
}


TOKEN_DEF_START
	TOKEN_DEF (TALK)
	TOKEN_DEF (TEMPLATE)
	TOKEN_DEF (ACTION)
	TOKEN_DEF (DEFAULT_SPRITESET_FILE)
	TOKEN_DEF (DEFAULT_SPRITESET)
	TOKEN_DEF (DEFAULT_SPRITE)
	TOKEN_DEF (EDITOR_PROPERTY)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
HRESULT CAdTalkDef::LoadBuffer(BYTE * Buffer, bool Complete)
{
	TOKEN_TABLE_START(commands)
		TOKEN_TABLE (TALK)
		TOKEN_TABLE (TEMPLATE)
		TOKEN_TABLE (ACTION)
		TOKEN_TABLE (DEFAULT_SPRITESET_FILE)
		TOKEN_TABLE (DEFAULT_SPRITESET)
		TOKEN_TABLE (DEFAULT_SPRITE)
		TOKEN_TABLE (EDITOR_PROPERTY)
	TOKEN_TABLE_END
	
	BYTE* params;
	int cmd;
	CBParser parser(Game);

	if(Complete){
		if(parser.GetCommand ((char**)&Buffer, commands, (char**)&params)!=TOKEN_TALK){
			Game->LOG(0, "'TALK' keyword expected.");
			return E_FAIL;
		}
		Buffer = params;
	}

	while ((cmd = parser.GetCommand ((char**)&Buffer, commands, (char**)&params)) > 0)
	{
		switch (cmd)
		{
			case TOKEN_TEMPLATE:
				if(FAILED(LoadFile((char*)params))) cmd = PARSERR_GENERIC;
			break;

			case TOKEN_ACTION:
			{
				CAdTalkNode* Node = new CAdTalkNode(Game);
				if(Node && SUCCEEDED(Node->LoadBuffer(params, false))) m_Nodes.Add(Node);
				else{
					SAFE_DELETE(Node);
					cmd = PARSERR_GENERIC;
				}
			}
			break;

			case TOKEN_DEFAULT_SPRITE:
				CBUtils::SetString(&m_DefaultSpriteFilename, (char*)params);
			break;

			case TOKEN_DEFAULT_SPRITESET_FILE:
				CBUtils::SetString(&m_DefaultSpriteSetFilename, (char*)params);
			break;

			case TOKEN_DEFAULT_SPRITESET:
			{
				SAFE_DELETE(m_DefaultSpriteSet);
				m_DefaultSpriteSet = new CAdSpriteSet(Game);
				if(!m_DefaultSpriteSet || FAILED(m_DefaultSpriteSet->LoadBuffer(params, false))){
					SAFE_DELETE(m_DefaultSpriteSet);
					cmd = PARSERR_GENERIC;
				}
			}				
			break;


			case TOKEN_EDITOR_PROPERTY:
				ParseEditorProperty(params, false);
			break;
		}
	}
	if (cmd == PARSERR_TOKENNOTFOUND){
		Game->LOG(0, "Syntax error in TALK definition");
		return E_FAIL;
	}

	if (cmd == PARSERR_GENERIC){
		Game->LOG(0, "Error loading TALK definition");
		return E_FAIL;
	}

	SAFE_DELETE(m_DefaultSprite);
	SAFE_DELETE(m_DefaultSpriteSet);

	if(m_DefaultSpriteFilename){
		m_DefaultSprite = new CBSprite(Game);
		if(!m_DefaultSprite || FAILED(m_DefaultSprite->LoadFile(m_DefaultSpriteFilename))) return E_FAIL;
	}

	if(m_DefaultSpriteSetFilename){
		m_DefaultSpriteSet = new CAdSpriteSet(Game);
		if(!m_DefaultSpriteSet || FAILED(m_DefaultSpriteSet->LoadFile(m_DefaultSpriteSetFilename))) return E_FAIL;
	}


	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdTalkDef::Persist(CBPersistMgr* PersistMgr){

	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_DefaultSprite));
	PersistMgr->Transfer(TMEMBER(m_DefaultSpriteFilename));
	PersistMgr->Transfer(TMEMBER(m_DefaultSpriteSet));
	PersistMgr->Transfer(TMEMBER(m_DefaultSpriteSetFilename));

	m_Nodes.Persist(PersistMgr);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdTalkDef::SaveAsText(CBDynBuffer* Buffer, int Indent)
{
	Buffer->PutTextIndent(Indent, "TALK {\n");
	if(m_DefaultSpriteFilename) Buffer->PutTextIndent(Indent+2, "DEFAULT_SPRITE=\"%s\"\n", m_DefaultSpriteFilename);

	if(m_DefaultSpriteSetFilename) Buffer->PutTextIndent(Indent+2, "DEFAULT_SPRITESET_FILE=\"%s\"\n", m_DefaultSpriteSetFilename);
	else if(m_DefaultSpriteSet) m_DefaultSpriteSet->SaveAsText(Buffer, Indent+2);

	for(int i=0; i<m_Nodes.GetSize(); i++){
		m_Nodes[i]->SaveAsText(Buffer, Indent+2);
		Buffer->PutTextIndent(Indent, "\n");
	}
	CBBase::SaveAsText(Buffer, Indent+2);
	
	Buffer->PutTextIndent(Indent, "}\n");

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdTalkDef::LoadDefaultSprite()
{
	if(m_DefaultSpriteFilename && !m_DefaultSprite){
		m_DefaultSprite = new CBSprite(Game);
		if(!m_DefaultSprite || FAILED(m_DefaultSprite->LoadFile(m_DefaultSpriteFilename))){
			SAFE_DELETE(m_DefaultSprite);
			return E_FAIL;
		}
		else return S_OK;
	}
	else if(m_DefaultSpriteSetFilename && !m_DefaultSpriteSet){
		m_DefaultSpriteSet = new CAdSpriteSet(Game);
		if(!m_DefaultSpriteSet || FAILED(m_DefaultSpriteSet->LoadFile(m_DefaultSpriteSetFilename))){
			SAFE_DELETE(m_DefaultSpriteSet);
			return E_FAIL;
		}
		else return S_OK;
	}	
	else return S_OK;
}


//////////////////////////////////////////////////////////////////////////
CBSprite* CAdTalkDef::GetDefaultSprite(TDirection Dir)
{
	LoadDefaultSprite();
	if(m_DefaultSprite)	return m_DefaultSprite;
	else if(m_DefaultSpriteSet) return m_DefaultSpriteSet->GetSprite(Dir);
	else return NULL;
}
