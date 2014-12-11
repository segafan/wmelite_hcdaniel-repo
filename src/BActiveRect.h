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

#ifndef __WmeBActiveRect_H__
#define __WmeBActiveRect_H__


class CBActiveRect:CBBase
{
public:	
	void ClipRect();
	bool m_Precise;
	float m_ZoomX;
	float m_ZoomY;
	CBSubFrame* m_Frame;
	CBObject* m_Owner;
	CBRegion* m_Region;
	int m_OffsetX;
	int m_OffsetY;
	RECT m_Rect;
	CBActiveRect(CBGame* inGameOwner=NULL);
	CBActiveRect(CBGame* inGameOwner, CBObject* Owner, CBSubFrame* Frame, int X, int Y, int Width, int Height, float ZoomX=100, float ZoomY=100, bool Precise=true);
	CBActiveRect(CBGame* inGame, CBObject* Owner, CBRegion* Region, int OffsetX, int OffsetY);
	virtual ~CBActiveRect();

};

#endif
