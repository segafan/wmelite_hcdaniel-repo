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
#include "BFontStorage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_PERSISTENT(CBFontStorage, true);

//////////////////////////////////////////////////////////////////////////
CBFontStorage::CBFontStorage(CBGame* inGame):CBBase(inGame)
{
	m_FTLibrary = NULL;
	InitFreeType();
}


//////////////////////////////////////////////////////////////////////////
CBFontStorage::~CBFontStorage()
{
	Cleanup(true);
}


//////////////////////////////////////////////////////////////////////////
void CBFontStorage::InitFreeType()
{
	FT_Error error = FT_Init_FreeType(&m_FTLibrary);
	if (error)
	{
		Game->LOG(0, "Error initializing FreeType library.");
	}
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBFontStorage::Cleanup(bool Warn)
{
	int i;

	for(i=0; i<m_Fonts.GetSize(); i++)
	{
		if(Warn) Game->LOG(0, "Removing orphan font '%s'", m_Fonts[i]->m_Filename);
		delete m_Fonts[i];
	}
	m_Fonts.RemoveAll();


	if (m_FTLibrary) FT_Done_FreeType(m_FTLibrary);


	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBFontStorage::InitLoop()
{
	for (int i = 0; i < m_Fonts.GetSize(); i++)
	{
		m_Fonts[i]->InitLoop();
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CBFont* CBFontStorage::AddFont(char *Filename)
{
	if(!Filename) return NULL;

	for(int i=0; i<m_Fonts.GetSize(); i++){
		if(CBPlatform::stricmp(m_Fonts[i]->m_Filename, Filename)==0){
			m_Fonts[i]->m_RefCount++;
			return m_Fonts[i];
		}
	}

	/*
	CBFont* font = new CBFont(Game);
	if(!font) return NULL;

	if(FAILED(font->LoadFile(Filename))){
		delete font;
		return NULL;
	}
	else {
		font->m_RefCount = 1;
		m_Fonts.Add(font);
		return font;
	}
	*/
	CBFont* font = CBFont::CreateFromFile(Game, Filename);
	if(font)
	{
		font->m_RefCount = 1;
		m_Fonts.Add(font);
	}
	return font;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFontStorage::RemoveFont(CBFont *Font)
{
	if(!Font) return E_FAIL;

	for(int i=0; i<m_Fonts.GetSize(); i++){
		if(m_Fonts[i]==Font){
			m_Fonts[i]->m_RefCount--;
			if(m_Fonts[i]->m_RefCount <= 0){
				delete m_Fonts[i];
				m_Fonts.RemoveAt(i);
			}
			break;
		}
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFontStorage::Persist(CBPersistMgr* PersistMgr){

	if(!PersistMgr->m_Saving) Cleanup(false);

	PersistMgr->Transfer(TMEMBER(Game));
	m_Fonts.Persist(PersistMgr);

	if(!PersistMgr->m_Saving) InitFreeType();

	return S_OK;
}
