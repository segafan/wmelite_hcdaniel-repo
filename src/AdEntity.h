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

#ifndef __WmeAdEntity_H__
#define __WmeAdEntity_H__


class CAdEntity : public CAdTalkHolder
{
public:
#if !defined(__LINUX__) && !defined(__ANDROID__)
	CVidTheoraPlayer* m_Theora;
#endif
	HRESULT SetSprite(char* Filename);
	int m_WalkToX;
	int m_WalkToY;
	TDirection m_WalkToDir;
	void SetItem(char* ItemName);
	char* m_Item;
	DECLARE_PERSISTENT(CAdEntity, CAdTalkHolder);
	void UpdatePosition();
	virtual int GetHeight();
	CBRegion* m_Region;
	virtual HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);
	virtual HRESULT Update();
	virtual HRESULT Display();
	CAdEntity(CBGame* inGame);
	virtual ~CAdEntity();
	HRESULT LoadFile(char * Filename);
	HRESULT LoadBuffer(BYTE* Buffer, bool Complete=true);
	TEntityType m_Subtype;

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();

};

#endif
