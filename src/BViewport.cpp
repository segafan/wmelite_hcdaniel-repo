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
#include "BViewport.h"



IMPLEMENT_PERSISTENT(CBViewport, false);

//////////////////////////////////////////////////////////////////////////
CBViewport::CBViewport(CBGame* inGame):CBBase(inGame)
{
	CBPlatform::SetRectEmpty(&m_Rect);
	m_MainObject = NULL;
	m_OffsetX = m_OffsetY = 0;
}


//////////////////////////////////////////////////////////////////////////
CBViewport::~CBViewport()
{

}


//////////////////////////////////////////////////////////////////////////
HRESULT CBViewport::Persist(CBPersistMgr* PersistMgr){

	PersistMgr->Transfer(TMEMBER(Game));

	PersistMgr->Transfer(TMEMBER(m_MainObject));
	PersistMgr->Transfer(TMEMBER(m_OffsetX));
	PersistMgr->Transfer(TMEMBER(m_OffsetY));
	PersistMgr->Transfer(TMEMBER(m_Rect));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBViewport::SetRect(int left, int top, int right, int bottom, bool NoCheck)
{
	if(!NoCheck){
		left = max(left, 0);
		top = max(top, 0);
		right = min(right, Game->m_Renderer->m_Width);
		bottom = min(bottom, Game->m_Renderer->m_Height);
	}
	
	CBPlatform::SetRect(&m_Rect, left, top, right, bottom);
	m_OffsetX = left;
	m_OffsetY = top;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
RECT* CBViewport::GetRect()
{
	return &m_Rect;
}


//////////////////////////////////////////////////////////////////////////
int CBViewport::GetWidth()
{
	return m_Rect.right - m_Rect.left;
}


//////////////////////////////////////////////////////////////////////////
int CBViewport::GetHeight()
{
	return m_Rect.bottom - m_Rect.top;
}
