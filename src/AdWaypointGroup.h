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

#ifndef __WmeAdWaypointGroup_H__
#define __WmeAdWaypointGroup_H__


class CAdWaypointGroup : public CBObject
{
public:
	float m_LastMimicScale;
	int m_LastMimicX;
	int m_LastMimicY;
	void Cleanup();
	HRESULT Mimic(CAdWaypointGroup* Wpt, float Scale=100.0f, int X=0, int Y=0);
	DECLARE_PERSISTENT(CAdWaypointGroup, CBObject);
	virtual HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);
	bool m_Active;
	CAdWaypointGroup(CBGame* inGame);
	HRESULT LoadFile(char* Filename);
	HRESULT LoadBuffer(BYTE* Buffer, bool Complete=true);
	virtual ~CAdWaypointGroup();
	CBArray<CBPoint*, CBPoint*> m_Points;
	int m_EditorSelectedPoint;
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
};

#endif
