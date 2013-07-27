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
#include "AdResponseContext.h"


IMPLEMENT_PERSISTENT(CAdResponseContext, false);

//////////////////////////////////////////////////////////////////////////
CAdResponseContext::CAdResponseContext(CBGame* inGame):CBBase(inGame)
{
	m_ID = 0;
	m_Context = NULL;
}


//////////////////////////////////////////////////////////////////////////
CAdResponseContext::~CAdResponseContext()
{
	SAFE_DELETE_ARRAY(m_Context);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdResponseContext::Persist(CBPersistMgr *PersistMgr)
{
	PersistMgr->Transfer(TMEMBER(Game));
	PersistMgr->Transfer(TMEMBER(m_Context));
	PersistMgr->Transfer(TMEMBER(m_ID));

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CAdResponseContext::SetContext(char* Context)
{
	SAFE_DELETE_ARRAY(m_Context);
	if(Context){
		m_Context = new char [strlen(Context)+1];
		if(m_Context) strcpy(m_Context, Context);
	}
}