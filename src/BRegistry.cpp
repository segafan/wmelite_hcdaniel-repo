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
#include <fstream>
#include "BRegistry.h"
#include "PathUtil.h"
#include "StringUtil.h"
#include "tinyxml.h"


//////////////////////////////////////////////////////////////////////////
CBRegistry::CBRegistry(CBGame* inGame):CBBase(inGame)
{
	m_IniName = NULL;

	SetIniName("./wme.ini");
	LoadValues(true);
}


//////////////////////////////////////////////////////////////////////////
CBRegistry::~CBRegistry()
{
	SaveValues();
	SAFE_DELETE_ARRAY(m_IniName);
}



//////////////////////////////////////////////////////////////////////////
AnsiString CBRegistry::ReadString(const AnsiString& subKey, const AnsiString& key, const AnsiString& init)
{
	AnsiString ret = "";

#ifdef __WIN32__
	// check ini file first (so what we can use project files on windows)	
	char buffer[32768];
	GetPrivateProfileString(subKey.c_str(), key.c_str(), init.c_str(), buffer, 32768, m_IniName);
	ret = AnsiString(buffer);

	if (buffer != init) return ret;
#endif

	bool found = false;
	ret = GetValue(m_LocalValues, subKey, key, found);
	if (!found) ret = GetValue(m_Values, subKey, key, found);
	if (!found) ret = init;

	return ret;
}


//////////////////////////////////////////////////////////////////////////
bool CBRegistry::WriteString(const AnsiString& subKey, const AnsiString& key, const AnsiString& value)
{
	m_Values[subKey][key] = value;	
	return true;
}


//////////////////////////////////////////////////////////////////////////
int CBRegistry::ReadInt(const AnsiString& subKey, const AnsiString& key, int init)
{
#ifdef __WIN32__
	int ret = GetPrivateProfileInt(subKey.c_str(), key.c_str(), init, m_IniName);
	if(ret != init) return ret;
#endif

	AnsiString val = ReadString(subKey, key, "");
	if (val.empty()) return init;
	else return atoi(val.c_str());
}


//////////////////////////////////////////////////////////////////////////
bool CBRegistry::WriteInt(const AnsiString& subKey, const AnsiString& key, int value)
{
	WriteString(subKey, key, StringUtil::ToString(value));
	return true;
}


//////////////////////////////////////////////////////////////////////////
bool CBRegistry::ReadBool(const AnsiString& subKey, const AnsiString& key, bool init)
{
	return (ReadInt(subKey, key, (int)init) != 0);
}


//////////////////////////////////////////////////////////////////////////
bool CBRegistry::WriteBool(const AnsiString& subKey, const AnsiString& key, bool value)
{
	return WriteInt(subKey, key, (int)value);
}


//////////////////////////////////////////////////////////////////////////
void CBRegistry::SetIniName(char* Name)
{
	SAFE_DELETE_ARRAY(m_IniName);

	if (strchr(Name, '\\') == NULL && strchr(Name, '/') == NULL)
	{
		m_IniName = new char [strlen(Name)+3];
		sprintf(m_IniName, "./%s", Name);
	}
	else
	{
		m_IniName = new char [strlen(Name)+1];
		strcpy(m_IniName, Name);
	}
}


//////////////////////////////////////////////////////////////////////////
char* CBRegistry::GetIniName()
{
	return m_IniName;
}

//////////////////////////////////////////////////////////////////////////
void CBRegistry::LoadValues(bool local)
{
	if (local) LoadXml("settings.xml", m_LocalValues);
	else LoadXml(PathUtil::Combine(Game->GetDataDir(), "settings.xml"), m_Values);
}

//////////////////////////////////////////////////////////////////////////
void CBRegistry::SaveValues()
{	
	SaveXml(PathUtil::Combine(Game->GetDataDir(), "settings.xml"), m_Values);
}

//////////////////////////////////////////////////////////////////////////
void CBRegistry::SetBasePath(const char* basePath)
{
	m_BasePath = PathUtil::GetFileNameWithoutExtension(basePath);

	LoadValues(false);
}

//////////////////////////////////////////////////////////////////////////
AnsiString CBRegistry::GetValue(PathValueMap& values, const AnsiString path, const AnsiString& key, bool& found)
{
	found = false;
	PathValueMap::iterator it = values.find(path);
	if (it == values.end()) return "";

	KeyValuePair pairs = (*it).second;
	KeyValuePair::iterator keyIt = pairs.find(key);
	if (keyIt == pairs.end()) return "";
	else
	{
		found = true;
		return (*keyIt).second;
	}
}

//////////////////////////////////////////////////////////////////////////
void CBRegistry::LoadXml(const AnsiString fileName, PathValueMap& values)
{
	TiXmlDocument doc(fileName);
	if (!doc.LoadFile()) return;

	TiXmlElement* rootElem = doc.RootElement();
	if (!rootElem || rootElem->ValueStr() != "Settings") return;

	for (TiXmlElement* pathElem = rootElem->FirstChildElement(); pathElem != NULL; pathElem = pathElem->NextSiblingElement())
	{
		for (TiXmlElement* keyElem = pathElem->FirstChildElement(); keyElem != NULL; keyElem = keyElem->NextSiblingElement())
		{
			values[pathElem->ValueStr()][keyElem->ValueStr()] = keyElem->GetText();
		}
	}
}


//////////////////////////////////////////////////////////////////////////
void CBRegistry::SaveXml(const AnsiString fileName, PathValueMap& values)
{
	CBUtils::CreatePath(fileName.c_str());

	TiXmlDocument doc;
	doc.LinkEndChild(new TiXmlDeclaration( "1.0", "utf-8", ""));

	TiXmlElement* root = new TiXmlElement("Settings");
	doc.LinkEndChild(root);

	PathValueMap::iterator pathIt;
	for (pathIt = m_Values.begin(); pathIt != m_Values.end(); ++pathIt)
	{
		TiXmlElement* pathElem = new TiXmlElement((*pathIt).first);
		root->LinkEndChild(pathElem);


		KeyValuePair pairs = (*pathIt).second;
		KeyValuePair::iterator keyIt;
		for (keyIt = pairs.begin(); keyIt != pairs.end(); ++keyIt)
		{
			TiXmlElement* keyElem = new TiXmlElement((*keyIt).first);
			pathElem->LinkEndChild(keyElem);

			keyElem->LinkEndChild(new TiXmlText((*keyIt).second));
		}
	}


	TiXmlPrinter printer;
	doc.Accept(&printer);

	std::ofstream stream;
	stream.open(fileName.c_str());

	if (!stream.is_open()) return;
	else
	{
		stream << printer.Str();
		stream.close();
	}
}
