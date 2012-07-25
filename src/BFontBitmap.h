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

#ifndef __WmeBFontBitmap_H__
#define __WmeBFontBitmap_H__


#include "BFont.h"

class CBFontBitmap : public CBFont
{
public:			
	DECLARE_PERSISTENT(CBFontBitmap, CBFont);
	HRESULT LoadBuffer(BYTE* Buffer);
	HRESULT LoadFile(char* Filename);
	virtual int GetTextWidth(BYTE* text, int MaxLength=-1);
	virtual int GetTextHeight(BYTE* text, int width);
	virtual void DrawText(BYTE* text, int x, int y, int width, TTextAlign align=TAL_LEFT, int max_height=-1, int MaxLenght=-1);
	virtual int GetLetterHeight();

	CBFontBitmap(CBGame* inGame);
	virtual ~CBFontBitmap();


	HRESULT GetWidths();
	CBSprite* m_Sprite;
	int m_WidthsFrame;
	bool m_FontextFix;
	int m_NumColumns;
	int m_TileHeight;
	int m_TileWidth;
	BYTE m_Widths[NUM_CHARACTERS];
	CBSubFrame* m_Subframe;
	bool m_WholeCell;

private:
	int GetCharWidth(BYTE Index);
	void DrawChar(BYTE c, int x, int y);

	int TextHeightDraw(BYTE* text, int x, int y, int width, TTextAlign align, bool draw, int max_height=-1, int MaxLenght=-1);

};

#endif
