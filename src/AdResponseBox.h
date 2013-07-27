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

#ifndef __WmeAdResponseBox_H__
#define __WmeAdResponseBox_H__


#include "BObject.h"

class CAdResponseBox : public CBObject  
{
public:
	CBObject* GetNextAccessObject(CBObject* CurrObject);
	CBObject* GetPrevAccessObject(CBObject* CurrObject);
	HRESULT GetObjects(CBArray<CUIObject*, CUIObject*>& Objects, bool InteractiveOnly);

	HRESULT HandleResponse(CAdResponse* Response);
	void SetLastResponseText(char* Text, char* TextOrig);
	char* m_LastResponseText;
	char* m_LastResponseTextOrig;
	DECLARE_PERSISTENT(CAdResponseBox, CBObject);
	CScScript* m_WaitingScript;
	virtual HRESULT Listen(CBScriptHolder* param1, DWORD param2);
	typedef enum{
		EVENT_PREV, EVENT_NEXT, EVENT_RESPONSE
	} TResponseEvent;

	HRESULT WeedResponses();
	HRESULT Display();
	int m_Spacing;
	int m_ScrollOffset;
	CBFont* m_FontHover;
	CBFont* m_Font;
	HRESULT CreateButtons();
	HRESULT InvalidateButtons();
	void ClearButtons();
	void ClearResponses();
	CAdResponseBox(CBGame* inGame);
	virtual ~CAdResponseBox();
	CBArray<CAdResponse*, CAdResponse*> m_Responses;
	CBArray<CUIButton*, CUIButton*> m_RespButtons;
	CUIWindow* m_Window;
	CUIWindow* m_ShieldWindow;
	bool m_Horizontal;
	RECT m_ResponseArea;
	int m_VerticalAlign;
	TTextAlign m_Align;
	HRESULT LoadFile(char* Filename);
	HRESULT LoadBuffer(BYTE* Buffer, bool Complete=true);
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent);
};

#endif
