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
#include "BActiveRect.h"


//////////////////////////////////////////////////////////////////////
CBActiveRect::CBActiveRect(CBGame* inGame):CBBase(inGame)
{
	CBPlatform::SetRectEmpty(&m_Rect);
	m_Owner = NULL;
	m_Frame = NULL;
	m_Region = NULL;
	m_ZoomX = 100;
	m_ZoomY = 100;
	m_OffsetX = m_OffsetY = 0;
	ClipRect();
}


//////////////////////////////////////////////////////////////////////
CBActiveRect::CBActiveRect(CBGame* inGame, CBObject* Owner, CBSubFrame* Frame, int X, int Y, int Width, int Height, float ZoomX, float ZoomY, bool Precise):CBBase(inGame)
{
	m_Owner = Owner;
	m_Frame = Frame;
	CBPlatform::SetRect(&m_Rect, X, Y, X+Width, Y+Height);
	m_ZoomX = ZoomX;
	m_ZoomY = ZoomY;
	m_Precise = Precise;
	m_Region = NULL;
	m_OffsetX = m_OffsetY = 0;
	ClipRect();
}

//////////////////////////////////////////////////////////////////////
CBActiveRect::CBActiveRect(CBGame* inGame, CBObject* Owner, CBRegion* Region, int OffsetX, int OffsetY):CBBase(inGame)
{
	m_Owner = Owner;	
	m_Region = Region;
	CBPlatform::CopyRect(&m_Rect, &Region->m_Rect);
	CBPlatform::OffsetRect(&m_Rect, -OffsetX, -OffsetY);
	m_ZoomX = 100;
	m_ZoomY = 100;
	m_Precise = true;
	m_Frame = NULL;	
	ClipRect();
	m_OffsetX = OffsetX;
	m_OffsetY = OffsetY;
}


//////////////////////////////////////////////////////////////////////
CBActiveRect::~CBActiveRect()
{
	m_Owner = NULL;
	m_Frame = NULL;
	m_Region = NULL;
}


//////////////////////////////////////////////////////////////////////////
void CBActiveRect::ClipRect()
{
	RECT rc;
	bool CustomViewport;
	Game->GetCurrentViewportRect(&rc, &CustomViewport);
	CBRenderer* Rend = Game->m_Renderer;

	if(!CustomViewport)
	{
		rc.left -= Rend->m_DrawOffsetX;
		rc.right -= Rend->m_DrawOffsetX;
		rc.top -= Rend->m_DrawOffsetY;
		rc.bottom -= Rend->m_DrawOffsetY;
	}

	if(rc.left > m_Rect.left) m_OffsetX = rc.left - m_Rect.left;
	if(rc.top  > m_Rect.top)  m_OffsetY = rc.top  - m_Rect.top;

	CBPlatform::IntersectRect(&m_Rect, &m_Rect, &rc);
}
