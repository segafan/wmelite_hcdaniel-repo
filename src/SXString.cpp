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
#include "SXString.h"
#include "SXArray.h"
#include "StringUtil.h"
#include <vector>


IMPLEMENT_PERSISTENT(CSXString, false);

//////////////////////////////////////////////////////////////////////////
CSXString::CSXString(CBGame* inGame, CScStack* Stack):CBScriptable(inGame)
{
	m_String = NULL;
	m_Capacity = 0;
	
	Stack->CorrectParams(1);
	CScValue* Val = Stack->Pop();

	if(Val->IsInt()){
		m_Capacity = max(0, Val->GetInt());
		if(m_Capacity>0){
			m_String = new char[m_Capacity];
			memset(m_String, 0, m_Capacity);
		}
	}
	else{
		SetStringVal(Val->GetString());
	}

	if(m_Capacity==0) SetStringVal("");
}


//////////////////////////////////////////////////////////////////////////
CSXString::~CSXString()
{
	if(m_String) delete [] m_String;
}


//////////////////////////////////////////////////////////////////////////
void CSXString::SetStringVal(const char *Val)
{
	int Len = strlen(Val);
	if(Len>=m_Capacity){
		m_Capacity = Len + 1;
		SAFE_DELETE_ARRAY(m_String);
		m_String = new char[m_Capacity];
		memset(m_String, 0, m_Capacity);
	}
	strcpy(m_String, Val);
}


//////////////////////////////////////////////////////////////////////////
char* CSXString::ScToString()
{
	if(m_String) return m_String;
	else return "[null string]";
}


//////////////////////////////////////////////////////////////////////////
void CSXString::ScSetString(const char *Val)
{
	SetStringVal(Val);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CSXString::ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name)
{
	//////////////////////////////////////////////////////////////////////////
	// Substring
	//////////////////////////////////////////////////////////////////////////
	if(strcmp(Name, "Substring")==0)
	{
		Stack->CorrectParams(2);
		int start = Stack->Pop()->GetInt();
		int end   = Stack->Pop()->GetInt();

		if(end < start) CBUtils::Swap(&start, &end);

		try
		{
			WideString str;
			if (Game->m_TextEncoding == TEXT_UTF8)
				str = StringUtil::Utf8ToWide(m_String);
			else
				str = StringUtil::AnsiToWide(m_String);

			WideString subStr = str.substr(start, end - start + 1);

			if (Game->m_TextEncoding == TEXT_UTF8)
				Stack->PushString(StringUtil::WideToUtf8(subStr).c_str());
			else
				Stack->PushString(StringUtil::WideToAnsi(subStr).c_str());
		}
		catch (std::exception&)
		{
			Stack->PushNULL();
		}

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Substr
	//////////////////////////////////////////////////////////////////////////
	else if(strcmp(Name, "Substr")==0)
	{
		Stack->CorrectParams(2);
		int start = Stack->Pop()->GetInt();

		CScValue* val = Stack->Pop();
		int len = val->GetInt();

		if (!val->IsNULL() && len <= 0)
		{
			Stack->PushString("");
			return S_OK;
		}

		if (val->IsNULL()) len = strlen(m_String) - start;

		try
		{
			WideString str;
			if (Game->m_TextEncoding == TEXT_UTF8)
				str = StringUtil::Utf8ToWide(m_String);
			else
				str = StringUtil::AnsiToWide(m_String);

			WideString subStr = str.substr(start, len);

			if (Game->m_TextEncoding == TEXT_UTF8)
				Stack->PushString(StringUtil::WideToUtf8(subStr).c_str());
			else
				Stack->PushString(StringUtil::WideToAnsi(subStr).c_str());
		}
		catch (std::exception&)
		{
			Stack->PushNULL();
		}

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// ToUpperCase
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ToUpperCase")==0)
	{
		Stack->CorrectParams(0);

		WideString str;
		if (Game->m_TextEncoding == TEXT_UTF8)
			str = StringUtil::Utf8ToWide(m_String);
		else
			str = StringUtil::AnsiToWide(m_String);

		StringUtil::ToUpperCase(str);

		if (Game->m_TextEncoding == TEXT_UTF8)
			Stack->PushString(StringUtil::WideToUtf8(str).c_str());
		else
			Stack->PushString(StringUtil::WideToAnsi(str).c_str());

		return S_OK;
	}
	
	//////////////////////////////////////////////////////////////////////////
	// ToLowerCase
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "ToLowerCase")==0)
	{
		Stack->CorrectParams(0);

		WideString str;
		if (Game->m_TextEncoding == TEXT_UTF8)
			str = StringUtil::Utf8ToWide(m_String);
		else
			str = StringUtil::AnsiToWide(m_String);

		StringUtil::ToLowerCase(str);

		if (Game->m_TextEncoding == TEXT_UTF8)
			Stack->PushString(StringUtil::WideToUtf8(str).c_str());
		else
			Stack->PushString(StringUtil::WideToAnsi(str).c_str());

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// IndexOf
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "IndexOf")==0)
	{
		Stack->CorrectParams(2);

		char* strToFind = Stack->Pop()->GetString();
		int index = Stack->Pop()->GetInt();

		WideString str;
		if (Game->m_TextEncoding == TEXT_UTF8)
			str = StringUtil::Utf8ToWide(m_String);
		else
			str = StringUtil::AnsiToWide(m_String);

		WideString toFind;
		if (Game->m_TextEncoding == TEXT_UTF8)
			toFind = StringUtil::Utf8ToWide(strToFind);
		else
			toFind = StringUtil::AnsiToWide(strToFind);

		int indexOf = StringUtil::IndexOf(str, toFind, index);
		Stack->PushInt(indexOf);

		return S_OK;
	}

	//////////////////////////////////////////////////////////////////////////
	// Split
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Split")==0)
	{
		Stack->CorrectParams(1);
		CScValue* Val = Stack->Pop();
		char Separators[MAX_PATH] = ",";
		if(!Val->IsNULL()) strcpy(Separators, Val->GetString());

		CSXArray* Array = new CSXArray(Game);
		if (!Array)
		{
			Stack->PushNULL();
			return S_OK;
		}


		WideString str;
		if (Game->m_TextEncoding == TEXT_UTF8)
			str = StringUtil::Utf8ToWide(m_String);
		else
			str = StringUtil::AnsiToWide(m_String);

		WideString delims;
		if (Game->m_TextEncoding == TEXT_UTF8)
			delims = StringUtil::Utf8ToWide(Separators);
		else
			delims = StringUtil::AnsiToWide(Separators);

		vector<WideString> parts;


		size_t start, pos;
		start = 0;
		do 
		{
			pos = str.find_first_of(delims, start);
			if (pos == start)
			{
				start = pos + 1;
			}
			else if (pos == WideString::npos)
			{
				parts.push_back(str.substr(start));
				break;
			}
			else
			{
				parts.push_back(str.substr(start, pos - start));
				start = pos + 1;
			}
			start = str.find_first_not_of(delims, start);

		} while (pos != WideString::npos);

		for (vector<WideString>::iterator it = parts.begin(); it != parts.end(); ++it)
		{
			WideString& part = (*it);

			if (Game->m_TextEncoding == TEXT_UTF8)
				Val = new CScValue(Game, StringUtil::WideToUtf8(part).c_str());
			else
				Val = new CScValue(Game, StringUtil::WideToAnsi(part).c_str());

			Array->Push(Val);
			SAFE_DELETE(Val);
		}

		Stack->PushNative(Array, false);
		return S_OK;
	}

	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
CScValue* CSXString::ScGetProperty(char *Name)
{
	m_ScValue->SetNULL();

	//////////////////////////////////////////////////////////////////////////
	// Type (RO)
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "Type")==0)
	{
		m_ScValue->SetString("string");
		return m_ScValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Length (RO)
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Length")==0)
	{
		if (Game->m_TextEncoding == TEXT_UTF8)
		{
			WideString wstr = StringUtil::Utf8ToWide(m_String);
			m_ScValue->SetInt(wstr.length());
		}
		else
			m_ScValue->SetInt(strlen(m_String));

		return m_ScValue;
	}
	//////////////////////////////////////////////////////////////////////////
	// Capacity
	//////////////////////////////////////////////////////////////////////////
	else if (strcmp(Name, "Capacity")==0)
	{
		m_ScValue->SetInt(m_Capacity);
		return m_ScValue;
	}

	else return m_ScValue;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CSXString::ScSetProperty(char *Name, CScValue *Value)
{
	//////////////////////////////////////////////////////////////////////////
	// Capacity
	//////////////////////////////////////////////////////////////////////////
	if (strcmp(Name, "Capacity")==0)
	{
		int NewCap = Value->GetInt();
		if (NewCap < strlen(m_String)+1) Game->LOG(0, "Warning: cannot lower string capacity");
		else if (NewCap != m_Capacity)
		{
			char* NewStr = new char[NewCap];
			if (NewStr)
			{
				memset(NewStr, 0, NewCap);
				strcpy(NewStr, m_String);
				SAFE_DELETE_ARRAY(m_String);
				m_String = NewStr;
				m_Capacity = NewCap;
			}
		}
		return S_OK;
	}

	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CSXString::Persist(CBPersistMgr* PersistMgr){

	CBScriptable::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_Capacity));

	if(PersistMgr->m_Saving){
		if(m_Capacity>0) PersistMgr->PutBytes((BYTE*)m_String, m_Capacity);
	}
	else{
		if(m_Capacity>0){
			m_String = new char[m_Capacity];
			PersistMgr->GetBytes((BYTE*)m_String, m_Capacity);
		}
		else m_String = NULL;
	}
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
int CSXString::ScCompare(CBScriptable *Val)
{
	return strcmp(m_String, ((CSXString*)Val)->m_String);
}
