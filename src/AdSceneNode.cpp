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
#include "AdSceneNode.h"


IMPLEMENT_PERSISTENT(CAdSceneNode, false);

//////////////////////////////////////////////////////////////////////////
CAdSceneNode::CAdSceneNode(CBGame* inGame):CBObject(inGame)
{
	m_Type = OBJECT_NONE;
	m_Region = NULL;
	m_Entity = NULL;
}


//////////////////////////////////////////////////////////////////////////
CAdSceneNode::~CAdSceneNode()
{
	Game->UnregisterObject(m_Region);
	m_Region = NULL;

	Game->UnregisterObject(m_Entity);
	m_Entity = NULL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSceneNode::SetEntity(CAdEntity *Entity)
{
	m_Type = OBJECT_ENTITY;
	m_Entity = Entity;
	return Game->RegisterObject(Entity);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSceneNode::SetRegion(CAdRegion *Region)
{
	m_Type = OBJECT_REGION;
	m_Region = Region;
	return Game->RegisterObject(Region);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSceneNode::Persist(CBPersistMgr* PersistMgr){

	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_Entity));
	PersistMgr->Transfer(TMEMBER(m_Region));
	PersistMgr->Transfer(TMEMBER_INT(m_Type));

	return S_OK;
}
