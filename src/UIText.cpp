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
#include "UIText.h"


IMPLEMENT_PERSISTENT(CUIText, false);

//////////////////////////////////////////////////////////////////////////
CUIText::CUIText(CBGame* inGame):CUIObject(inGame)
{
	m_TextAlign = TAL_LEFT;
	m_VerticalAlign = VAL_CENTER;
	m_Type = UI_STATIC;
	m_CanFocus = false;
}


//////////////////////////////////////////////////////////////////////////
CUIText::~CUIText()
{

}


//////////////////////////////////////////////////////////////////////////
HRESULT CUIText::Display(int OffsetX, int OffsetY)
{
	if(!m_Visible) return S_OK;


	CBFont* font = m_Font;
	if(!font) font = Game->m_SystemFont;

	if(m_Back) m_Back->Display(OffsetX + m_PosX, OffsetY + m_PosY, m_Width, m_Height);
	if(m_Image) m_Image->Draw(OffsetX + m_PosX, OffsetY + m_PosY, NULL);

	if(font && m_Text){
		int text_offset;
		switch(m_VerticalAlign)
		{
			case VAL_TOP:
				text_offset = 0;
				break;
			case VAL_BOTTOM:
				text_offset = m_Height - font->GetTextHeight((BYTE*)m_Text, m_Width);
				break;
			default: text_offset = (m_Height - font->GetTextHeight((BYTE*)m_Text, m_Width)) / 2;
		}
		font->DrawText((BYTE*)m_Text, OffsetX + m_PosX, OffsetY + m_PosY + text_offset, m_Width, m_TextAlign, m_Height);
	}

	//Game->m_Renderer->m_RectList.Add(new CBActiveRect(Game, this, NULL, OffsetX + m_PosX, OffsetY + m_PosY, m_Width, m_Height, 100, 100, false));

	return S_OK;
}



//////////////////////////////////////////////////////////////////////////
HRESULT CUIText::LoadFile(char* Filename)
{
	BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename);
	if(Buffer==NULL){
		Game->LOG(0, "CUIText::LoadFile failed for file '%s'", Filename);
		return E_FAIL;
	}

	HRESULT ret;

	m_Filename = new char [strlen(Filename)+1];
	strcpy(m_Filename, Filename);

	if(FAILED(ret = LoadBuffer(Buffer, true))) Game->LOG(0, "Error parsing STATIC file '%s'", Filename);

	delete [] Buffer;

	return ret;
}


TOKEN_DEF_START
	TOKEN_DEF (STATIC)
	TOKEN_DEF (TEMPLATE)
	TOKEN_DEF (DISABLED)
	TOKEN_DEF (VISIBLE)
	TOKEN_DEF (BACK)
	TOKEN_DEF (IMAGE)
	TOKEN_DEF (FONT)
	TOKEN_DEF (TEXT_ALIGN)
	TOKEN_DEF (VERTICAL_ALIGN)
	TOKEN_DEF (TEXT)
	TOKEN_DEF (X)
	TOKEN_DEF (Y)
	TOKEN_DEF (WIDTH)
	TOKEN_DEF (HEIGHT)
	TOKEN_DEF (CURSOR)
	TOKEN_DEF (NAME)
	TOKEN_DEF (SCRIPT)
	TOKEN_DEF (CAPTION)
	TOKEN_DEF (PARENT_NOTIFY)
	TOKEN_DEF (EDITOR_PROPERTY)
TOKEN_DEF_END
//////////////////////////////////////////////////////////////////////////
HRESULT CUIText::LoadBuffer(BYTE* Buffer, bool Complete)
{
	TOKEN_TABLE_START(commands)
		TOKEN_TABLE (STATIC)
		TOKEN_TABLE (TEMPLATE)
		TOKEN_TABLE (DISABLED)
		TOKEN_TABLE (VISIBLE)
		TOKEN_TABLE (BACK)
		TOKEN_TABLE (IMAGE)
		TOKEN_TABLE (FONT)
		TOKEN_TABLE (TEXT_ALIGN)
		TOKEN_TABLE (VERTICAL_ALIGN)
		TOKEN_TABLE (TEXT)
		TOKEN_TABLE (X)
		TOKEN_TABLE (Y)
		TOKEN_TABLE (WIDTH)
		TOKEN_TABLE (HEIGHT)
		TOKEN_TABLE (CURSOR)
		TOKEN_TABLE (NAME)
		TOKEN_TABLE (SCRIPT)
		TOKEN_TABLE (CAPTION)
		TOKEN_TABLE (PARENT_NOTIFY)
		TOKEN_TABLE (EDITOR_PROPERTY)
	TOKEN_TABLE_END
	
	BYTE* params;
	int cmd=2;
	CBParser parser(Game);

	if(Complete){
		if(parser.GetCommand ((char**)&Buffer, commands, (char**)&params)!=TOKEN_STATIC){
			Game->LOG(0, "'STATIC' keyword expected.");
			return E_FAIL;
		}
		Buffer = params;
	}

	while (cmd > 0 && (cmd = parser.GetCommand ((char**)&Buffer, commands, (char**)&params)) > 0)
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

			case TOKEN_BACK:
				SAFE_DELETE(m_Back);
				m_Back = new CUITiledImage(Game);
				if(!m_Back || FAILED(m_Back->LoadFile((char*)params))){
					SAFE_DELETE(m_Back);
					cmd = PARSERR_GENERIC;
				}
			break;

			case TOKEN_IMAGE:
				SAFE_DELETE(m_Image);
				m_Image = new CBSprite(Game);
				if(!m_Image || FAILED(m_Image->LoadFile((char*)params))){
					SAFE_DELETE(m_Image);
					cmd = PARSERR_GENERIC;
				}
			break;

			case TOKEN_FONT:
				if(m_Font) Game->m_FontStorage->RemoveFont(m_Font);
				m_Font = Game->m_FontStorage->AddFont((char*)params);
				if(!m_Font) cmd = PARSERR_GENERIC;
			break;

			case TOKEN_TEXT:
				SetText((char*)params);
				Game->m_StringTable->Expand(&m_Text);
			break;

			case TOKEN_TEXT_ALIGN:
				if(CBPlatform::stricmp((char*)params, "left")==0) m_TextAlign = TAL_LEFT;
				else if(CBPlatform::stricmp((char*)params, "right")==0) m_TextAlign = TAL_RIGHT;
				else m_TextAlign = TAL_CENTER;
			break;

			case TOKEN_VERTICAL_ALIGN:
				if(CBPlatform::stricmp((char*)params, "top")==0) m_VerticalAlign = VAL_TOP;
				else if(CBPlatform::stricmp((char*)params, "bottom")==0) m_VerticalAlign = VAL_BOTTOM;
				else m_VerticalAlign = VAL_CENTER;
			break;

			case TOKEN_X:
				parser.ScanStr((char*)params, "%d", &m_PosX);
			break;

			case TOKEN_Y:
				parser.ScanStr((char*)params, "%d", &m_PosY);
			break;

			case TOKEN_WIDTH:
				parser.ScanStr((char*)params, "%d", &m_Width);
			break;

			case TOKEN_HEIGHT:
				parser.ScanStr((char*)params, "%d", &m_Height);
			break;

			case TOKEN_CURSOR:
				SAFE_DELETE(m_Cursor);
				m_Cursor = new CBSprite(Game);
				if(!m_Cursor || FAILED(m_Cursor->LoadFile((char*)params))){
					SAFE_DELETE(m_Cursor);
					cmd = PARSERR_GENERIC;
				}
			break;

			case TOKEN_SCRIPT:
				AddScript((char*)params);
			break;

			case TOKEN_PARENT_NOTIFY:
				parser.ScanStr((char*)params, "%b", &m_ParentNotify);
			break;

			case TOKEN_DISABLED:
				parser.ScanStr((char*)params, "%b", &m_Disable);
			break;

			case TOKEN_VISIBLE:
				parser.ScanStr((char*)params, "%b", &m_Visible);
			break;

			case TOKEN_EDITOR_PROPERTY:
				ParseEditorProperty(params, false);
			break;		
		}
	}
	if (cmd == PARSERR_TOKENNOTFOUND){
		Game->LOG(0, "Syntax error in STATIC definition");
		return E_FAIL;
	}
	if (cmd == PARSERR_GENERIC){
		Game->LOG(0, "Error loading STATIC definition");
		return E_FAIL;
	}

	CorrectSize();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CUIText::SaveAsText(CBDynBuffer* Buffer, int Indent)
{
	Buffer->PutTextIndent(Indent, "STATIC\n");
	Buffer->PutTextIndent(Indent, "{\n");

	Buffer->PutTextIndent(Indent+2, "NAME=\"%s\"\n", m_Name);
	Buffer->PutTextIndent(Indent+2, "CAPTION=\"%s\"\n", GetCaption());

	Buffer->PutTextIndent(Indent+2, "\n");

	if(m_Back && m_Back->m_Filename)
		Buffer->PutTextIndent(Indent+2, "BACK=\"%s\"\n", m_Back->m_Filename);

	if(m_Image && m_Image->m_Filename)
		Buffer->PutTextIndent(Indent+2, "IMAGE=\"%s\"\n", m_Image->m_Filename);

	if(m_Font && m_Font->m_Filename)
		Buffer->PutTextIndent(Indent+2, "FONT=\"%s\"\n", m_Font->m_Filename);

	if(m_Cursor && m_Cursor->m_Filename)
		Buffer->PutTextIndent(Indent+2, "CURSOR=\"%s\"\n", m_Cursor->m_Filename);

	if(m_Text)
		Buffer->PutTextIndent(Indent+2, "TEXT=\"%s\"\n", m_Text);

	switch(m_TextAlign)
	{
		case TAL_LEFT:
			Buffer->PutTextIndent(Indent+2, "TEXT_ALIGN=\"%s\"\n", "left");
			break;
		case TAL_RIGHT:
			Buffer->PutTextIndent(Indent+2, "TEXT_ALIGN=\"%s\"\n", "right");
			break;
		case TAL_CENTER:
			Buffer->PutTextIndent(Indent+2, "TEXT_ALIGN=\"%s\"\n", "center");
			break;
	}

	switch(m_VerticalAlign)
	{
		case VAL_TOP:
			Buffer->PutTextIndent(Indent+2, "VERTICAL_ALIGN=\"%s\"\n", "top");
			break;
		case VAL_BOTTOM:
			Buffer->PutTextIndent(Indent+2, "VERTICAL_ALIGN=\"%s\"\n", "bottom");
			break;
		case VAL_CENTER:
			Buffer->PutTextIndent(Indent+2, "VERTICAL_ALIGN=\"%s\"\n", "center");
			break;
	}

	Buffer->PutTextIndent(Indent+2, "\n");

	Buffer->PutTextIndent(Indent+2, "X=%d\n", m_PosX);
	Buffer->PutTextIndent(Indent+2, "Y=%d\n", m_PosY);
	Buffer->PutTextIndent(Indent+2, "WIDTH=%d\n", m_Width);
	Buffer->PutTextIndent(Indent+2, "HEIGHT=%d\n", m_Height);

	Buffer->PutTextIndent(Indent+2, "DISABLED=%s\n", m_Disable?"TRUE":"FALSE");
	Buffer->PutTextIndent(Indent+2, "VISIBLE=%s\n", m_Visible?"TRUE":"FALSE");
	Buffer->PutTextIndent(Indent+2, "PARENT_NOTIFY=%s\n", m_ParentNotify?"TRUE":"FALSE");

	Buffer->PutTextIndent(Indent+2, "\n");

	// scripts
	for(int i=0; i<m_Scripts.GetSize(); i++)
	{
		Buffer->PutTextIndent(Indent+2, "SCRIPT=\"%s\"\n", m_Scripts[i]->m_Filename);
	}

	Buffer->PutTextIndent(Indent+2, "\n");

	// editor properties
	CBBase::SaveAsText(Buffer, Indent+2);

	Buffer->PutTextIndent(Indent, "}\n");
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// high level scripting interface
//////////////////////////////////////////////////////////////////////////
HRESULT CUIText::ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name)
{
	//////////////////////////////////////////////////////////////////////////
	// SizeToFit
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "SizeToFit")==0){
		Stack->CorrectParams(0);
		SizeToFit();
		Stack->PushNULL();
		return S_OK;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// HeightToFit
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "HeightToFit")==0){
		Stack->CorrectParams(0);
		if(m_Font && m_Text) m_Height = m_Font->GetTextHeight((BYTE*)m_Text, m_Width);
		Stack->PushNULL();
		return S_OK;
	}
	
	else return CUIObject::ScCallMethod(Script, Stack, ThisStack, Name);
}


//////////////////////////////////////////////////////////////////////////
CScValue* CUIText::ScGetProperty(char *Name)
{
	m_ScValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "Type")==0){
		m_ScValue->SetString("static");
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// TextAlign
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "TextAlign")==0){
		m_ScValue->SetInt(m_TextAlign);
		return m_ScValue;
	}

	//////////////////////////////////////////////////////////////////////////
	// VerticalAlign
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "VerticalAlign")==0){
		m_ScValue->SetInt(m_VerticalAlign);
		return m_ScValue;
	}
	
	else return CUIObject::ScGetProperty(Name);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CUIText::ScSetProperty(char *Name, CScValue *Value)
{
	//////////////////////////////////////////////////////////////////////////
	// TextAlign
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "TextAlign")==0){
		int i = Value->GetInt();
		if(i<0 || i>=NUM_TEXT_ALIGN) i = 0;
		m_TextAlign = (TTextAlign)i;
		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// VerticalAlign
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "VerticalAlign")==0){
		int i = Value->GetInt();
		if(i<0 || i>=NUM_VERTICAL_ALIGN) i = 0;
		m_VerticalAlign = (TVerticalAlign)i;
		return S_OK;
	}

	else return CUIObject::ScSetProperty(Name, Value);
}


//////////////////////////////////////////////////////////////////////////
char* CUIText::ScToString()
{
	return "[static]";
}



//////////////////////////////////////////////////////////////////////////
HRESULT CUIText::Persist(CBPersistMgr* PersistMgr){

	CUIObject::Persist(PersistMgr);
	PersistMgr->Transfer(TMEMBER_INT(m_TextAlign));
	PersistMgr->Transfer(TMEMBER_INT(m_VerticalAlign));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CUIText::SizeToFit()
{
	if(m_Font && m_Text){
		m_Width = m_Font->GetTextWidth((BYTE*)m_Text);
		m_Height = m_Font->GetTextHeight((BYTE*)m_Text, m_Width);
	}
	return S_OK;
}
