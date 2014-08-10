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
#include "BRegion.h"



IMPLEMENT_PERSISTENT(CBRegion, false);

//////////////////////////////////////////////////////////////////////////
CBRegion::CBRegion(CBGame* inGame):CBObject(inGame)
{
	m_Active = true;
	m_EditorSelectedPoint = -1;
	m_LastMimicScale = -1;
	m_LastMimicX = m_LastMimicY = INT_MIN;

	CBPlatform::SetRectEmpty(&m_Rect);
}


//////////////////////////////////////////////////////////////////////////
CBRegion::~CBRegion()
{
	Cleanup();
}


//////////////////////////////////////////////////////////////////////////
void CBRegion::Cleanup()
{
	for(int i=0; i<m_Points.GetSize(); i++) delete m_Points[i];
	m_Points.RemoveAll();

	CBPlatform::SetRectEmpty(&m_Rect);
	m_EditorSelectedPoint = -1;
}


//////////////////////////////////////////////////////////////////////////
bool CBRegion::CreateRegion()
{
	return SUCCEEDED(GetBoundingRect(&m_Rect));
}


//////////////////////////////////////////////////////////////////////////
bool CBRegion::PointInRegion(int X, int Y)
{
	if(m_Points.GetSize() < 3) return false;

	POINT pt;
	pt.x = X; pt.y = Y;

	RECT rect;
	rect.left = X-1;
	rect.right = X+2;
	rect.top = Y-1;
	rect.bottom = Y+2;
	
	if(CBPlatform::PtInRect(&m_Rect, pt)) return PtInPolygon(X, Y);
	else return false;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRegion::LoadFile(char * Filename)
{
	BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename);
	if(Buffer==NULL){
		Game->LOG(0, "CBRegion::LoadFile failed for file '%s'", Filename);
		return E_FAIL;
	}

	HRESULT ret;

	m_Filename = new char [strlen(Filename)+1];
	strcpy(m_Filename, Filename);

	if(FAILED(ret = LoadBuffer(Buffer, true))) Game->LOG(0, "Error parsing REGION file '%s'", Filename);


	delete [] Buffer;

	return ret;
}


TOKEN_DEF_START
	TOKEN_DEF (REGION)
	TOKEN_DEF (TEMPLATE)
	TOKEN_DEF (NAME)
	TOKEN_DEF (ACTIVE)
	TOKEN_DEF (POINT)
	TOKEN_DEF (CAPTION)
	TOKEN_DEF (SCRIPT)
	TOKEN_DEF (EDITOR_SELECTED_POINT)
	TOKEN_DEF (PROPERTY)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
HRESULT CBRegion::LoadBuffer(BYTE * Buffer, bool Complete)
{
	TOKEN_TABLE_START(commands)
		TOKEN_TABLE (REGION)
		TOKEN_TABLE (TEMPLATE)
		TOKEN_TABLE (NAME)
		TOKEN_TABLE (ACTIVE)
		TOKEN_TABLE (POINT)
		TOKEN_TABLE (CAPTION)
		TOKEN_TABLE (SCRIPT)
		TOKEN_TABLE (EDITOR_SELECTED_POINT)
		TOKEN_TABLE (PROPERTY)
	TOKEN_TABLE_END
	
	BYTE* params;
	int cmd;
	CBParser parser(Game);

	if(Complete){
		if(parser.GetCommand ((char**)&Buffer, commands, (char**)&params)!=TOKEN_REGION)
		{
			Game->LOG(0, "'REGION' keyword expected.");
			return E_FAIL;
		}
		Buffer = params;
	}

	int i;

	for(i=0; i<m_Points.GetSize(); i++) delete m_Points[i];
	m_Points.RemoveAll();

	while ((cmd = parser.GetCommand ((char**)&Buffer, commands, (char**)&params)) > 0)
	{
		switch (cmd)
		{
			case TOKEN_TEMPLATE:
				if(FAILED(LoadFile((char*)params))) cmd = PARSERR_GENERIC;
			break;

			case TOKEN_NAME:
				SetName((char*)params);
			break;

			case TOKEN_CAPTION:
				SetCaption((char*)params);
			break;

			case TOKEN_ACTIVE:
				parser.ScanStr((char*)params, "%b", &m_Active);
			break;

			case TOKEN_POINT:
			{
				int x,y;
				parser.ScanStr((char*)params, "%d,%d", &x, &y);
				m_Points.Add(new CBPoint(x, y));
			}
			break;

			case TOKEN_SCRIPT:
				AddScript((char*)params);
			break;

			case TOKEN_EDITOR_SELECTED_POINT:
				parser.ScanStr((char*)params, "%d", &m_EditorSelectedPoint);
			break;		

			case TOKEN_PROPERTY:
				ParseProperty(params, false);
			break;
		}
	}
	if (cmd == PARSERR_TOKENNOTFOUND){
		Game->LOG(0, "Syntax error in REGION definition");
		return E_FAIL;
	}

	CreateRegion();

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT CBRegion::ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name)
{

	//////////////////////////////////////////////////////////////////////////
	// AddPoint
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "AddPoint")==0)
	{
		Stack->CorrectParams(2);
		int X = Stack->Pop()->GetInt();
		int Y = Stack->Pop()->GetInt();

		m_Points.Add(new CBPoint(X, Y));
		CreateRegion();

		Stack->PushBool(true);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// InsertPoint
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "InsertPoint")==0)
	{
		Stack->CorrectParams(3);
		int Index = Stack->Pop()->GetInt();
		int X = Stack->Pop()->GetInt();
		int Y = Stack->Pop()->GetInt();

		if(Index >= 0 && Index < m_Points.GetSize())
		{
			m_Points.InsertAt(Index, new CBPoint(X, Y));
			CreateRegion();

			Stack->PushBool(true);
		}
		else Stack->PushBool(false);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// SetPoint
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "SetPoint")==0)
	{
		Stack->CorrectParams(3);
		int Index = Stack->Pop()->GetInt();
		int X = Stack->Pop()->GetInt();
		int Y = Stack->Pop()->GetInt();

		if(Index >= 0 && Index < m_Points.GetSize())
		{
			m_Points[Index]->x = X;
			m_Points[Index]->y = Y;
			CreateRegion();

			Stack->PushBool(true);
		}
		else Stack->PushBool(false);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// RemovePoint
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "RemovePoint")==0)
	{
		Stack->CorrectParams(1);
		int Index = Stack->Pop()->GetInt();

		if(Index >= 0 && Index < m_Points.GetSize())
		{
			SAFE_DELETE(m_Points[Index]);
			m_Points.RemoveAt(Index);
			CreateRegion();

			Stack->PushBool(true);
		}
		else Stack->PushBool(false);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// GetPoint
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "GetPoint")==0)
	{
		Stack->CorrectParams(1);
		int Index = Stack->Pop()->GetInt();

		if(Index >= 0 && Index < m_Points.GetSize())
		{
			CScValue* Val = Stack->GetPushValue();
			if(Val)
			{
				Val->SetProperty("X", m_Points[Index]->x);
				Val->SetProperty("Y", m_Points[Index]->y);
			}
		}
		else Stack->PushNULL();

		return S_OK;
	}

	else return CBObject::ScCallMethod(Script, Stack, ThisStack, Name);
}


//////////////////////////////////////////////////////////////////////////
CScValue* CBRegion::ScGetProperty(char *Name)
{
	m_ScValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "Type")==0)
	{
		m_ScValue->SetString("region");
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "Name")==0)
	{
		m_ScValue->SetString(m_Name);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// Active
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "Active")==0)
	{
		m_ScValue->SetBool(m_Active);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// NumPoints
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "NumPoints")==0)
	{
		m_ScValue->SetInt(m_Points.GetSize());
		return m_ScValue;
	}
	
	else return CBObject::ScGetProperty(Name);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRegion::ScSetProperty(char *Name, CScValue *Value)
{
	//////////////////////////////////////////////////////////////////////////
	// Name
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "Name")==0)
	{
		SetName(Value->GetString());
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Active
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "Active")==0)
	{
		m_Active = Value->GetBool();
		return S_OK;
	}

	else return CBObject::ScSetProperty(Name, Value);
}


//////////////////////////////////////////////////////////////////////////
char* CBRegion::ScToString()
{
	return "[region]";
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRegion::SaveAsText(CBDynBuffer *Buffer, int Indent, char* NameOverride)
{
	if(!NameOverride) Buffer->PutTextIndent(Indent, "REGION {\n");
	else Buffer->PutTextIndent(Indent, "%s {\n", NameOverride);

	Buffer->PutTextIndent(Indent+2, "NAME=\"%s\"\n", m_Name);
	Buffer->PutTextIndent(Indent+2, "CAPTION=\"%s\"\n", GetCaption());
	Buffer->PutTextIndent(Indent+2, "ACTIVE=%s\n", m_Active?"TRUE":"FALSE");
	Buffer->PutTextIndent(Indent+2, "EDITOR_SELECTED_POINT=%d\n", m_EditorSelectedPoint);

	int i;

	for(i=0; i<m_Scripts.GetSize(); i++)
	{
		Buffer->PutTextIndent(Indent+2, "SCRIPT=\"%s\"\n", m_Scripts[i]->m_Filename);
	}

	for(i=0; i<m_Points.GetSize(); i++)
	{
		Buffer->PutTextIndent(Indent+2, "POINT {%d,%d}\n", m_Points[i]->x, m_Points[i]->y);
	}

	if(m_ScProp) m_ScProp->SaveAsText(Buffer, Indent+2);

	Buffer->PutTextIndent(Indent, "}\n\n");

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRegion::Persist(CBPersistMgr* PersistMgr){

	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_Active));
	PersistMgr->Transfer(TMEMBER(m_EditorSelectedPoint));
	PersistMgr->Transfer(TMEMBER(m_LastMimicScale));
	PersistMgr->Transfer(TMEMBER(m_LastMimicX));
	PersistMgr->Transfer(TMEMBER(m_LastMimicY));
	m_Points.Persist(PersistMgr);
	
	return S_OK;
}


typedef struct {
   double x,y;
} dPoint;

//////////////////////////////////////////////////////////////////////////
bool CBRegion::PtInPolygon(int X, int Y)
{
	if(m_Points.GetSize() < 3) return false;

	int counter = 0;
	int i;
	double xinters;
	dPoint p, p1, p2;

	p.x = (double)X;
	p.y = (double)Y;
	
	p1.x = (double)m_Points[0]->x;
	p1.y = (double)m_Points[0]->y;

	for (i=1; i<=m_Points.GetSize(); i++)
	{
		p2.x = (double)m_Points[i % m_Points.GetSize()]->x;
		p2.y = (double)m_Points[i % m_Points.GetSize()]->y;

		if (p.y > min(p1.y,p2.y))
		{
			if (p.y <= max(p1.y,p2.y))
			{
				if (p.x <= max(p1.x,p2.x))
				{
					if (p1.y != p2.y)
					{
						xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
						if (p1.x == p2.x || p.x <= xinters)
							counter++;
					}
				}
			}
		}
		p1 = p2;
	}
	
	if (counter % 2 == 0)
		return false;
	else
		return true;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRegion::GetBoundingRect(RECT *Rect)
{
	if(m_Points.GetSize()==0) CBPlatform::SetRectEmpty(Rect);
	else
	{
		int MinX=INT_MAX, MinY=INT_MAX, MaxX=INT_MIN, MaxY=INT_MIN;

		for(int i=0; i<m_Points.GetSize(); i++)
		{
			MinX = min(MinX, m_Points[i]->x);
			MinY = min(MinY, m_Points[i]->y);

			MaxX = max(MaxX, m_Points[i]->x);
			MaxY = max(MaxY, m_Points[i]->y);
		}
		CBPlatform::SetRect(Rect, MinX, MinY, MaxX, MaxY);
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBRegion::Mimic(CBRegion *Region, float Scale, int X, int Y)
{
	if(Scale==m_LastMimicScale && X==m_LastMimicX && Y==m_LastMimicY) return S_OK;

	Cleanup();

	for(int i=0; i<Region->m_Points.GetSize(); i++)
	{
		int x, y;

		x = (int)((float)Region->m_Points[i]->x * Scale / 100.0f);
		y = (int)((float)Region->m_Points[i]->y * Scale / 100.0f);

		m_Points.Add(new CBPoint(x+X, y+Y));
	}

	m_LastMimicScale = Scale;
	m_LastMimicX = X;
	m_LastMimicY = Y;

	return CreateRegion()?S_OK:E_FAIL;
}
