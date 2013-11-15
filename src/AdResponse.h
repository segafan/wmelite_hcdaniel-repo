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

#ifndef __WmeAdResponse_H__
#define __WmeAdResponse_H__


#include "BObject.h"

class CAdResponse : public CBObject
{
public:	
	DECLARE_PERSISTENT(CAdResponse, CBObject);
	HRESULT SetIcon(char* Filename);
	HRESULT SetFont(char* Filename);
	HRESULT SetIconHover(char* Filename);
	HRESULT SetIconPressed(char* Filename);
	void SetText(char* Text);
	int m_ID;
	CBSprite* m_Icon;
	CBSprite* m_IconHover;
	CBSprite* m_IconPressed;
	CBFont* m_Font;
	char* m_Text;
	char* m_TextOrig;
	CAdResponse(CBGame* inGame);
	virtual ~CAdResponse();
	TResponseType m_ResponseType;

};

#endif
