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
#include "AdSpriteSet.h"


IMPLEMENT_PERSISTENT(CAdSpriteSet, false);

//////////////////////////////////////////////////////////////////////////
CAdSpriteSet::CAdSpriteSet(CBGame* inGame, CBObject* Owner):CBObject(inGame)
{
	m_Owner = Owner;

	for(int i=0; i<NUM_DIRECTIONS; i++)
		m_Sprites[i] = NULL;
}


//////////////////////////////////////////////////////////////////////////
CAdSpriteSet::~CAdSpriteSet()
{
	for(int i=0; i<NUM_DIRECTIONS; i++){
		SAFE_DELETE(m_Sprites[i]);
	}

	m_Owner = NULL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSpriteSet::LoadFile(char * Filename, int LifeTime, TSpriteCacheType CacheType)
{
	BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename);
	if(Buffer==NULL){
		Game->LOG(0, "CAdSpriteSet::LoadFile failed for file '%s'", Filename);
		return E_FAIL;
	}

	HRESULT ret;

	if(FAILED(ret = LoadBuffer(Buffer, true))) Game->LOG(0, "Error parsing SPRITESET file '%s'", Filename);

	delete [] Buffer;

	return ret;
}


TOKEN_DEF_START
	TOKEN_DEF (SPRITESET)
	TOKEN_DEF (NAME)
	TOKEN_DEF (UP_LEFT)
	TOKEN_DEF (DOWN_LEFT)
	TOKEN_DEF (LEFT)
	TOKEN_DEF (UP_RIGHT)
	TOKEN_DEF (DOWN_RIGHT)
	TOKEN_DEF (RIGHT)
	TOKEN_DEF (UP)
	TOKEN_DEF (DOWN)
	TOKEN_DEF (TEMPLATE)
	TOKEN_DEF (EDITOR_PROPERTY)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
HRESULT CAdSpriteSet::LoadBuffer(BYTE * Buffer, bool Complete, int LifeTime, TSpriteCacheType CacheType)
{
	TOKEN_TABLE_START(commands)
		TOKEN_TABLE (SPRITESET)
		TOKEN_TABLE (NAME)
		TOKEN_TABLE (UP_LEFT)
		TOKEN_TABLE (DOWN_LEFT)
		TOKEN_TABLE (LEFT)
		TOKEN_TABLE (UP_RIGHT)
		TOKEN_TABLE (DOWN_RIGHT)
		TOKEN_TABLE (RIGHT)
		TOKEN_TABLE (UP)
		TOKEN_TABLE (DOWN)
		TOKEN_TABLE (TEMPLATE)
		TOKEN_TABLE (EDITOR_PROPERTY)
	TOKEN_TABLE_END
	
	BYTE* params;
	int cmd;
	CBParser parser(Game);

	if(Complete){
		if(parser.GetCommand ((char**)&Buffer, commands, (char**)&params)!=TOKEN_SPRITESET){
			Game->LOG(0, "'SPRITESET' keyword expected.");
			return E_FAIL;
		}
		Buffer = params;
	}

	CBSprite* spr=NULL;
	while ((cmd = parser.GetCommand ((char**)&Buffer, commands, (char**)&params)) > 0)
	{
		switch (cmd)
		{
			case TOKEN_TEMPLATE:
				if(FAILED(LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
			break;

			case TOKEN_NAME:
				SetName((char*)params);
			break;

			case TOKEN_LEFT:
				SAFE_DELETE(m_Sprites[DI_LEFT]);
				spr = new CBSprite(Game, m_Owner);
				if(!spr || FAILED(spr->LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
				else m_Sprites[DI_LEFT] = spr;
			break;

			case TOKEN_RIGHT:
				SAFE_DELETE(m_Sprites[DI_RIGHT]);
				spr = new CBSprite(Game, m_Owner);
				if(!spr || FAILED(spr->LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
				else m_Sprites[DI_RIGHT] = spr;
			break;

			case TOKEN_UP:
				SAFE_DELETE(m_Sprites[DI_UP]);
				spr = new CBSprite(Game, m_Owner);
				if(!spr || FAILED(spr->LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
				else m_Sprites[DI_UP] = spr;
			break;

			case TOKEN_DOWN:
				SAFE_DELETE(m_Sprites[DI_DOWN]);
				spr = new CBSprite(Game, m_Owner);
				if(!spr || FAILED(spr->LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
				else m_Sprites[DI_DOWN] = spr;
			break;

			case TOKEN_UP_LEFT:
				SAFE_DELETE(m_Sprites[DI_UPLEFT]);
				spr = new CBSprite(Game, m_Owner);
				if(!spr || FAILED(spr->LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
				else m_Sprites[DI_UPLEFT] = spr;
			break;

			case TOKEN_UP_RIGHT:
				SAFE_DELETE(m_Sprites[DI_UPRIGHT]);
				spr = new CBSprite(Game, m_Owner);
				if(!spr || FAILED(spr->LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
				else m_Sprites[DI_UPRIGHT] = spr;
			break;

			case TOKEN_DOWN_LEFT:
				SAFE_DELETE(m_Sprites[DI_DOWNLEFT]);
				spr = new CBSprite(Game, m_Owner);
				if(!spr || FAILED(spr->LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
				else m_Sprites[DI_DOWNLEFT] = spr;
			break;

			case TOKEN_DOWN_RIGHT:
				SAFE_DELETE(m_Sprites[DI_DOWNRIGHT]);
				spr = new CBSprite(Game, m_Owner);
				if(!spr || FAILED(spr->LoadFile((char*)params, LifeTime, CacheType))) cmd = PARSERR_GENERIC;
				else m_Sprites[DI_DOWNRIGHT] = spr;
			break;

			case TOKEN_EDITOR_PROPERTY:
				ParseEditorProperty(params, false);
			break;
		}
	}
	if (cmd == PARSERR_TOKENNOTFOUND){
		Game->LOG(0, "Syntax error in SPRITESET definition");
		return E_FAIL;
	}

	if (cmd == PARSERR_GENERIC){
		Game->LOG(0, "Error loading SPRITESET definition");
		if(spr) delete spr;
		return E_FAIL;
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSpriteSet::Persist(CBPersistMgr* PersistMgr){

	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_Owner));
	for(int i=0; i<NUM_DIRECTIONS; i++){
		PersistMgr->Transfer("", &m_Sprites[i]);
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
CBSprite* CAdSpriteSet::GetSprite(TDirection Direction)
{
	int Dir = (int)Direction;
	if(Dir<0) Dir = 0;
	if(Dir>=NUM_DIRECTIONS) Dir = NUM_DIRECTIONS - 1;

	CBSprite* ret = NULL;

	// find nearest set sprite
	int i;
	int NumSteps = 0;
	for(i=Dir, NumSteps=0; i>=0; i--){
		if(m_Sprites[i]!=NULL){
			ret = m_Sprites[i];
			NumSteps = Dir - i;
			break;
		}
	}

	for(i=Dir; i<NUM_DIRECTIONS; i++){
		if(m_Sprites[i]!=NULL){
			if(ret==NULL || NumSteps > i - Dir) return m_Sprites[i];
			else return ret;
		}
	}

	return ret;
}



//////////////////////////////////////////////////////////////////////////
HRESULT CAdSpriteSet::SaveAsText(CBDynBuffer* Buffer, int Indent)
{
	Buffer->PutTextIndent(Indent, "SPRITESET {\n");
	if(m_Name) Buffer->PutTextIndent(Indent+2, "NAME=\"%s\"\n", m_Name);
	for(int i=0; i<NUM_DIRECTIONS; i++){
		if(m_Sprites[i]){
			switch(i){
				case DI_UP:        Buffer->PutTextIndent(Indent+2, "UP=\"%s\"\n",         m_Sprites[i]->m_Filename); break;
				case DI_UPRIGHT:   Buffer->PutTextIndent(Indent+2, "UP_RIGHT=\"%s\"\n",   m_Sprites[i]->m_Filename); break;
				case DI_RIGHT:     Buffer->PutTextIndent(Indent+2, "RIGHT=\"%s\"\n",      m_Sprites[i]->m_Filename); break;
				case DI_DOWNRIGHT: Buffer->PutTextIndent(Indent+2, "DOWN_RIGHT=\"%s\"\n", m_Sprites[i]->m_Filename); break;
				case DI_DOWN:      Buffer->PutTextIndent(Indent+2, "DOWN=\"%s\"\n",       m_Sprites[i]->m_Filename); break;
				case DI_DOWNLEFT:  Buffer->PutTextIndent(Indent+2, "DOWN_LEFT=\"%s\"\n",  m_Sprites[i]->m_Filename); break;
				case DI_LEFT:      Buffer->PutTextIndent(Indent+2, "LEFT=\"%s\"\n",       m_Sprites[i]->m_Filename); break;
				case DI_UPLEFT:    Buffer->PutTextIndent(Indent+2, "UP_LEFT=\"%s\"\n",    m_Sprites[i]->m_Filename); break;
			}
		}
	}
	
	CBBase::SaveAsText(Buffer, Indent+2);
	
	Buffer->PutTextIndent(Indent, "}\n");

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CAdSpriteSet::ContainsSprite(CBSprite* Sprite)
{
	if(!Sprite) return NULL;

	for(int i=0; i<NUM_DIRECTIONS; i++)
	{
		if(m_Sprites[i]==Sprite) return true;
	}
	return false;
}
