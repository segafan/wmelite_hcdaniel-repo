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
#include "AdPathPoint.h"


IMPLEMENT_PERSISTENT(CAdPathPoint, false);

//////////////////////////////////////////////////////////////////////////
CAdPathPoint::CAdPathPoint()
{
	x = y = 0;
	m_Distance = 0;

	m_Marked = false;
	m_Origin = NULL;
}


//////////////////////////////////////////////////////////////////////////
CAdPathPoint::CAdPathPoint(int initX, int initY, int initDistance)
{
	x = initX;
	y = initY;
	m_Distance = initDistance;

	m_Marked = false;
	m_Origin = NULL;
}


//////////////////////////////////////////////////////////////////////////
CAdPathPoint::~CAdPathPoint()
{
	m_Origin = NULL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdPathPoint::Persist(CBPersistMgr* PersistMgr){
	
	CBPoint::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_Distance));
	PersistMgr->Transfer(TMEMBER(m_Marked));
	PersistMgr->Transfer(TMEMBER(m_Origin));

	return S_OK;
}