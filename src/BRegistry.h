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

#ifndef __WmeBRegistry_H__
#define __WmeBRegistry_H__


#include "BBase.h"

class CBRegistry : public CBBase  
{
public:
	void SetIniName(char* Name);
	char* GetIniName();
	bool WriteBool(const AnsiString& subKey, const AnsiString& key, bool Value);
	bool ReadBool(const AnsiString& subKey, const AnsiString& key, bool init = false);
	bool WriteInt(const AnsiString& subKey, const AnsiString& key, int value);
	int ReadInt(const AnsiString& subKey, const AnsiString& key, int init = 0);
	bool WriteString(const AnsiString& subKey, const AnsiString& key, const AnsiString& value);
	AnsiString ReadString(const AnsiString& subKey, const AnsiString& key, const AnsiString& init = "");
	CBRegistry(CBGame* inGame);
	virtual ~CBRegistry();

	void SetBasePath(const char* basePath);
	AnsiString GetBasePath() const { return m_BasePath; }

	void LoadValues(bool local);
	void SaveValues();

private:
	char* m_IniName;

	typedef map<AnsiString, AnsiString> KeyValuePair;
	typedef map<AnsiString, KeyValuePair> PathValueMap;

	PathValueMap m_LocalValues;
	PathValueMap m_Values;

	AnsiString m_BasePath;

	void LoadXml(const AnsiString fileName, PathValueMap& values);
	void SaveXml(const AnsiString fileName, PathValueMap& values);

	AnsiString GetValue(PathValueMap& values, const AnsiString path, const AnsiString& key, bool& found);
};

#endif
