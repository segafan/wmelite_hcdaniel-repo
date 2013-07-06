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

#ifndef __WmeBFontStorage_H__
#define __WmeBFontStorage_H__


#include "BBase.h"

#include <ft2build.h>
#include FT_FREETYPE_H


class CBFont;

class CBFontStorage : public CBBase  
{
public:
	DECLARE_PERSISTENT(CBFontStorage, CBBase);
	HRESULT Cleanup(bool Warn=false);	
	HRESULT RemoveFont(CBFont* Font);
	CBFont* AddFont(char* Filename);
	CBFontStorage(CBGame* inGame);
	virtual ~CBFontStorage();
	CBArray<CBFont*, CBFont*> m_Fonts;
	HRESULT InitLoop();

	FT_Library GetFTLibrary() const { return m_FTLibrary; }

private:
	void InitFreeType();
	FT_Library m_FTLibrary;
};

#endif
