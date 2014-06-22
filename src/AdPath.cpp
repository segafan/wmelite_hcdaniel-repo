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
#include "AdPath.h"


IMPLEMENT_PERSISTENT(CAdPath, false);

//////////////////////////////////////////////////////////////////////////
CAdPath::CAdPath(CBGame* inGame):CBBase(inGame)
{
	m_CurrIndex = -1;
	m_Ready = false;
}


//////////////////////////////////////////////////////////////////////////
CAdPath::~CAdPath()
{
	Reset();
}


//////////////////////////////////////////////////////////////////////////
void CAdPath::Reset()
{
	for(int i=0; i<m_Points.GetSize(); i++)
		delete m_Points[i];

	m_Points.RemoveAll();
	m_CurrIndex = -1;
	m_Ready = false;
}


//////////////////////////////////////////////////////////////////////////
CBPoint* CAdPath::GetFirst()
{
	if(m_Points.GetSize() > 0){
		m_CurrIndex = 0;
		return m_Points[m_CurrIndex];
	}
	else return NULL;
}


//////////////////////////////////////////////////////////////////////////
CBPoint* CAdPath::GetNext()
{
	m_CurrIndex++;
	if(m_CurrIndex < m_Points.GetSize()) return m_Points[m_CurrIndex];
	else return NULL;
}


//////////////////////////////////////////////////////////////////////////
CBPoint* CAdPath::GetCurrent()
{
	if(m_CurrIndex >= 0 && m_CurrIndex < m_Points.GetSize()) return m_Points[m_CurrIndex];
	else return NULL;
}


//////////////////////////////////////////////////////////////////////////
void CAdPath::AddPoint(CBPoint *point)
{
	m_Points.Add(point);
}


//////////////////////////////////////////////////////////////////////////
bool CAdPath::SetReady(bool ready)
{
	bool orig = m_Ready;
	m_Ready = ready;

	return orig;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdPath::Persist(CBPersistMgr* PersistMgr){
	
	PersistMgr->Transfer(TMEMBER(Game));

	PersistMgr->Transfer(TMEMBER(m_CurrIndex));
	m_Points.Persist(PersistMgr);
	PersistMgr->Transfer(TMEMBER(m_Ready));

	return S_OK;
}
