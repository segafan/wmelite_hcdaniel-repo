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

#ifndef __WmeBBase_H__
#define __WmeBBase_H__


#include <map>
#include <string>

class CBGame;
class CBDynBuffer;

class CBBase  
{
public:
	bool m_Persistable;
	HRESULT SetEditorProp(const char* PropName, const char* PropValue);
	const char* GetEditorProp(const char* PropName, const char* InitVal=NULL);
	CBBase(TDynamicConstructor, TDynamicConstructor){};
	HRESULT ParseEditorProperty(BYTE* Buffer, bool Complete=true);
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent=0);
	CBBase();
	CBGame* Game;
	CBBase(CBGame* GameOwner);
	virtual ~CBBase();

	map<string, string> m_EditorProps;
	map<string, string>::iterator m_EditorPropsIter;
};

#endif
