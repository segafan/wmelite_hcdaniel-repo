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

#ifndef __WmeUITiledImage_H__
#define __WmeUITiledImage_H__


#include "UIObject.h"

class CUITiledImage : public CBObject  
{
public:
	DECLARE_PERSISTENT(CUITiledImage, CBObject);
	void CorrectSize(int* Width, int* Height);
	HRESULT LoadFile(char * Filename);
	HRESULT LoadBuffer(BYTE * Buffer, bool Complete=true);
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent);

	HRESULT Display(int X, int Y, int Width, int Height);
	CUITiledImage(CBGame* inGame=NULL);
	virtual ~CUITiledImage();
	CBSubFrame* m_Image;
	RECT m_UpLeft;
	RECT m_UpMiddle;
	RECT m_UpRight;
	RECT m_MiddleLeft;
	RECT m_MiddleMiddle;
	RECT m_MiddleRight;
	RECT m_DownLeft;
	RECT m_DownMiddle;
	RECT m_DownRight;
};

#endif
