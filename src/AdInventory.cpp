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
#include "AdInventory.h"


IMPLEMENT_PERSISTENT(CAdInventory, false);

//////////////////////////////////////////////////////////////////////////
CAdInventory::CAdInventory(CBGame* inGame):CBObject(inGame)
{
	m_ScrollOffset = 0;
}


//////////////////////////////////////////////////////////////////////////
CAdInventory::~CAdInventory()
{
	m_TakenItems.RemoveAll(); // ref only
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdInventory::InsertItem(char *Name, char *InsertAfter)
{
	if(Name==NULL) return E_FAIL;

	CAdItem* item = ((CAdGame*)Game)->GetItemByName(Name);	
	if(item==NULL) return E_FAIL;

	int InsertIndex=-1;
	for(int i=0; i<m_TakenItems.GetSize(); i++){
		if(CBPlatform::stricmp(m_TakenItems[i]->m_Name, Name)==0){
			m_TakenItems.RemoveAt(i);
			i--;
			continue;
		}
		if(InsertAfter && CBPlatform::stricmp(m_TakenItems[i]->m_Name, InsertAfter)==0) InsertIndex = i+1;
	}


	if(InsertIndex==-1) m_TakenItems.Add(item);
	else m_TakenItems.InsertAt(InsertIndex, item);

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdInventory::RemoveItem(char *Name)
{
	if(Name==NULL) return E_FAIL;

	for(int i=0; i<m_TakenItems.GetSize(); i++){
		if(CBPlatform::stricmp(m_TakenItems[i]->m_Name, Name)==0){
			if(((CAdGame*)Game)->m_SelectedItem==m_TakenItems[i]) ((CAdGame*)Game)->m_SelectedItem = NULL;
			m_TakenItems.RemoveAt(i);
			return S_OK;
		}
	}

	return E_FAIL;
}



//////////////////////////////////////////////////////////////////////////
HRESULT CAdInventory::RemoveItem(CAdItem* Item)
{
	if(Item==NULL) return E_FAIL;

	for(int i=0; i<m_TakenItems.GetSize(); i++){
		if(m_TakenItems[i]==Item){
			if(((CAdGame*)Game)->m_SelectedItem==m_TakenItems[i]) ((CAdGame*)Game)->m_SelectedItem = NULL;
			m_TakenItems.RemoveAt(i);
			return S_OK;
		}
	}

	return E_FAIL;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CAdInventory::Persist(CBPersistMgr* PersistMgr){

	CBObject::Persist(PersistMgr);
	
	m_TakenItems.Persist(PersistMgr);
	PersistMgr->Transfer(TMEMBER(m_ScrollOffset));

	return S_OK;
}