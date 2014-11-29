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
#include "BStringTable.h"
#include "StringUtil.h"


//////////////////////////////////////////////////////////////////////////
CBStringTable::CBStringTable(CBGame* inGame):CBBase(inGame)
{

}


//////////////////////////////////////////////////////////////////////////
CBStringTable::~CBStringTable()
{
	// delete strings
	m_Strings.clear();

}


//////////////////////////////////////////////////////////////////////////
HRESULT CBStringTable::AddString(const char *Key, const char *Val, bool ReportDuplicities)
{
	if(Key==NULL || Val==NULL) return E_FAIL;
	
	if(CBPlatform::stricmp(Key, "@right-to-left")==0)
	{
		Game->m_TextRTL = true;
		return S_OK;
	}

	string final_key = Key;
	StringUtil::ToLowerCase(final_key);

	m_StringsIter = m_Strings.find(final_key);
	if(m_StringsIter != m_Strings.end() && ReportDuplicities) Game->LOG(0, "  Warning: Duplicate definition of string '%s'.", final_key.c_str());

	m_Strings[final_key] = Val;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
char* CBStringTable::GetKey(const char* Str)
{
	if(Str==NULL || Str[0]!='/') return NULL;

	char* value = strchr((char*)Str+1, '/');
	if(value==NULL) return NULL;

	char* key = new char[value-Str];
	strncpy(key, Str+1, value-Str-1);
	key[value-Str-1] = '\0';
	CBPlatform::strlwr(key);

	char* new_str;

	m_StringsIter = m_Strings.find(key);
	if(m_StringsIter != m_Strings.end())
	{
		new_str = new char[m_StringsIter->second.length()+1];
		strcpy(new_str, m_StringsIter->second.c_str());
		if(strlen(new_str)>0 && new_str[0]=='/' && strchr(new_str+1, '/'))
		{
			delete [] key;
			char* Ret = GetKey(new_str);
			delete [] new_str;
			return Ret;
		}
		else
		{
			delete [] new_str;
			return key;
		}
	}
	else{
		return key;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBStringTable::Expand(char** Str, bool ForceExpand)
{
	if(Game->m_DoNotExpandStrings && !ForceExpand) return;

	if(Str==NULL || *Str==NULL || *Str[0]!='/') return;

	char* value = strchr(*Str+1, '/');
	if(value==NULL) return;

	char* key = new char[value-*Str];
	strncpy(key, *Str+1, value-*Str-1);
	key[value-*Str-1] = '\0';
	CBPlatform::strlwr(key);

	value++;

	char* new_str;

	m_StringsIter = m_Strings.find(key);
	if(m_StringsIter != m_Strings.end()){
		new_str = new char[m_StringsIter->second.length()+1];
		strcpy(new_str, m_StringsIter->second.c_str());
	}
	else{
		new_str = new char[strlen(value)+1];
		strcpy(new_str, value);
	}
	
	delete [] key;
	delete [] *Str;
	*Str = new_str;

	if(strlen(*Str)>0 && *Str[0]=='/') Expand(Str, ForceExpand);
}


//////////////////////////////////////////////////////////////////////////
const char* CBStringTable::ExpandStatic(const char *String, bool ForceExpand)
{
	if(Game->m_DoNotExpandStrings && !ForceExpand) return String;

	if(String==NULL || String[0] == '\0' || String[0]!='/') return String;

	const char* value = strchr(String+1, '/');
	if(value==NULL) return String;

	char* key = new char[value-String];
	strncpy(key, String+1, value-String-1);
	key[value-String-1] = '\0';
	CBPlatform::strlwr(key);

	value++;

	const char* new_str;

	m_StringsIter = m_Strings.find(key);
	if(m_StringsIter != m_Strings.end()){
		new_str = m_StringsIter->second.c_str();
	}
	else{
		new_str = value;
	}
	
	delete [] key;

	if(strlen(new_str)>0 && new_str[0]=='/') return ExpandStatic(new_str, ForceExpand);
	else return new_str;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBStringTable::LoadFile(char *Filename, bool ClearOld)
{
	Game->LOG(0, "Loading string table...");
	
	if(ClearOld) m_Strings.clear();
	
	DWORD Size;
	BYTE* Buffer = Game->m_FileManager->ReadWholeFile(Filename, &Size);
	if(Buffer==NULL){
		Game->LOG(0, "CBStringTable::LoadFile failed for file '%s'", Filename);
		return E_FAIL;
	}

	int Pos = 0;

	if(Size>3 && Buffer[0]==0xEF && Buffer[1]==0xBB && Buffer[2]==0xBF)
	{
		Pos += 3;
		if(Game->m_TextEncoding!=TEXT_UTF8)
		{
			Game->m_TextEncoding = TEXT_UTF8;
			//Game->m_TextEncoding = TEXT_ANSI;
			Game->LOG(0, "  UTF8 file detected, switching to UTF8 text encoding");
		}
	}
	else Game->m_TextEncoding = TEXT_ANSI;

	int LineLength = 0;
	while(Pos<Size)
	{
		LineLength = 0;
		while(Pos+LineLength < Size && Buffer[Pos+LineLength]!='\n' && Buffer[Pos+LineLength]!='\0') LineLength++;
		
		int RealLength = LineLength - (Pos+LineLength>=Size?0:1);
		char* line = new char[RealLength+1];
		strncpy(line, (char*)&Buffer[Pos], RealLength);
		line[RealLength] = '\0';
		char* value = strchr(line, '\t');
		if(value==NULL) value = strchr(line, ' ');

		if(line[0]!=';')
		{
			if(value!=NULL)
			{
				value[0] = '\0';
				value++;
				for(int i=0; i<strlen(value); i++)
				{
					if(value[i]=='|') value[i] = '\n';
				}
				AddString(line, value, ClearOld);
			}
			else if(line[0]!='\0') AddString(line, "", ClearOld);
		}

		delete [] line;
		Pos+=LineLength+1;
	}

	delete [] Buffer;

	Game->LOG(0, "  %d strings loaded", m_Strings.size());

	return S_OK;
}
