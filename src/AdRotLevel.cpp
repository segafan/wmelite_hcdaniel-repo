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
#include "AdRotLevel.h"


IMPLEMENT_PERSISTENT(CAdRotLevel, false);


//////////////////////////////////////////////////////////////////////////
CAdRotLevel::CAdRotLevel(CBGame* inGame):CBObject(inGame)
{
	m_PosX = 0;
	m_Rotation = 0.0f;
}


//////////////////////////////////////////////////////////////////////////
CAdRotLevel::~CAdRotLevel()
{

}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdRotLevel::LoadFile(char * Filename)
{
	BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename);
	if(Buffer==NULL){
		Game->LOG(0, "CAdRotLevel::LoadFile failed for file '%s'", Filename);
		return E_FAIL;
	}

	HRESULT ret;

	m_Filename = new char [strlen(Filename)+1];
	strcpy(m_Filename, Filename);

	if(FAILED(ret = LoadBuffer(Buffer, true))) Game->LOG(0, "Error parsing ROTATION_LEVEL file '%s'", Filename);


	delete [] Buffer;

	return ret;
}


TOKEN_DEF_START
	TOKEN_DEF (ROTATION_LEVEL)
	TOKEN_DEF (TEMPLATE)
	TOKEN_DEF (X)
	TOKEN_DEF (ROTATION)
	TOKEN_DEF (EDITOR_PROPERTY)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
HRESULT CAdRotLevel::LoadBuffer(BYTE * Buffer, bool Complete)
{
	TOKEN_TABLE_START(commands)
		TOKEN_TABLE (ROTATION_LEVEL)
		TOKEN_TABLE (TEMPLATE)
		TOKEN_TABLE (X)
		TOKEN_TABLE (ROTATION)
		TOKEN_TABLE (EDITOR_PROPERTY)
	TOKEN_TABLE_END
	
	BYTE* params;
	int cmd;
	CBParser parser(Game);

	if(Complete){
		if(parser.GetCommand ((char**)&Buffer, commands, (char**)&params)!=TOKEN_ROTATION_LEVEL){
			Game->LOG(0, "'ROTATION_LEVEL' keyword expected.");
			return E_FAIL;
		}
		Buffer = params;
	}

	while ((cmd = parser.GetCommand ((char**)&Buffer, commands, (char**)&params)) > 0)
	{
		switch (cmd)
		{
			case TOKEN_TEMPLATE:
				if(FAILED(LoadFile((char*)params))) cmd = PARSERR_GENERIC;
			break;

			case TOKEN_X:
				parser.ScanStr((char*)params, "%d", &m_PosX);
			break;

			case TOKEN_ROTATION:
			{
				int i;
				parser.ScanStr((char*)params, "%d", &i);
				m_Rotation = (float)i;
			}
			break;

			case TOKEN_EDITOR_PROPERTY:
				ParseEditorProperty(params, false);
			break;
		}
	}
	if (cmd == PARSERR_TOKENNOTFOUND){
		Game->LOG(0, "Syntax error in ROTATION_LEVEL definition");
		return E_FAIL;
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdRotLevel::SaveAsText(CBDynBuffer *Buffer, int Indent)
{
	Buffer->PutTextIndent(Indent, "ROTATION_LEVEL {\n");
	Buffer->PutTextIndent(Indent+2, "X=%d\n", m_PosX);
	Buffer->PutTextIndent(Indent+2, "ROTATION=%d\n", (int)m_Rotation);
	CBBase::SaveAsText(Buffer, Indent+2);
	Buffer->PutTextIndent(Indent, "}\n");

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdRotLevel::Persist(CBPersistMgr* PersistMgr){

	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_Rotation));

	return S_OK;
}
