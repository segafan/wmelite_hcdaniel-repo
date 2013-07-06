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
#include "BFont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_PERSISTENT(CBFont, false);

//////////////////////////////////////////////////////////////////////
CBFont::CBFont(CBGame* inGame):CBObject(inGame)
{

}


//////////////////////////////////////////////////////////////////////
CBFont::~CBFont()
{
}


//////////////////////////////////////////////////////////////////////
void CBFont::DrawText(BYTE * text, int x, int y, int width, TTextAlign align, int max_height, int MaxLenght)
{
}


//////////////////////////////////////////////////////////////////////
int CBFont::GetTextHeight(BYTE * text, int width)
{
	return 0;
}


//////////////////////////////////////////////////////////////////////
int CBFont::GetTextWidth(BYTE * text, int MaxLenght)
{
	return 0;
}

/*
//////////////////////////////////////////////////////////////////////
HRESULT CBFont::LoadFile(char * Filename)
{
	BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename);
	if(Buffer==NULL){
		Game->LOG(0, "CBFont::LoadFile failed for file '%s'", Filename);
		return E_FAIL;
	}

	HRESULT ret;

	m_Filename = new char [strlen(Filename)+1];
	strcpy(m_Filename, Filename);

	if(FAILED(ret = LoadBuffer(Buffer))) Game->LOG(0, "Error parsing FONT file '%s'", Filename);

	delete [] Buffer;

	return ret;
}


TOKEN_DEF_START
  TOKEN_DEF (FONT)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////
HRESULT CBFont::LoadBuffer(BYTE * Buffer)
{
	TOKEN_TABLE_START(commands)
		TOKEN_TABLE (FONT)
	TOKEN_TABLE_END
	
	char* params;
	int cmd;
	CBParser parser(Game);

	if(parser.GetCommand ((char**)&Buffer, commands, (char**)&params)!=TOKEN_FONT){
		Game->LOG(0, "'FONT' keyword expected.");
		return E_FAIL;
	}
	Buffer = (BYTE*)params;

	while ((cmd = parser.GetCommand ((char**)&Buffer, commands, (char**)&params)) > 0)
	{
		switch (cmd)
		{
			case TOKEN_IMAGE:
				surface_file = (char*)params;
			break;

			case TOKEN_TRANSPARENT:
				parser.ScanStr(params, "%d,%d,%d", &r, &g, &b);
				custom_trans = true;
			break;
		}


	}
	if (cmd == PARSERR_TOKENNOTFOUND){
		Game->LOG(0, "Syntax error in FONT definition");
		return E_FAIL;
	}

	return S_OK;
}
*/

//////////////////////////////////////////////////////////////////////////
int CBFont::GetLetterHeight()
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFont::Persist(CBPersistMgr* PersistMgr){

	CBObject::Persist(PersistMgr);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
CBFont* CBFont::CreateFromFile(CBGame* Game, char* Filename)
{
	if(IsTrueType(Game, Filename))
	{
		CBFontTT* Font = new CBFontTT(Game);
		if(Font)
		{
			if(FAILED(Font->LoadFile(Filename)))
			{
				delete Font;
				return NULL;
			}
		}
		return Font;
	}
	else
	{
		CBFontBitmap* Font = new CBFontBitmap(Game);
		if(Font)
		{
			if(FAILED(Font->LoadFile(Filename)))
			{
				delete Font;
				return NULL;
			}
		}
		return Font;
	}
}


TOKEN_DEF_START
	TOKEN_DEF (FONT)
	TOKEN_DEF (TTFONT)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
bool CBFont::IsTrueType(CBGame* Game, char* Filename)
{
	TOKEN_TABLE_START(commands)
		TOKEN_TABLE (FONT)
		TOKEN_TABLE (TTFONT)
	TOKEN_TABLE_END


	BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename);
	if(Buffer==NULL) return false;

	BYTE* WorkBuffer = Buffer;

	char* params;
	CBParser parser(Game);

	bool Ret = false;
	if(parser.GetCommand ((char**)&WorkBuffer, commands, (char**)&params)==TOKEN_TTFONT)
		Ret = true;

	delete [] Buffer;
	return Ret;
}
