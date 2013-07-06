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

#ifndef __WmeSysClassRegistry_H__
#define __WmeSysClassRegistry_H__


class CBGame;

class CSysClassRegistry  
{
public:
	static CSysClassRegistry* GetInstance();

	HRESULT EnumInstances(SYS_INSTANCE_CALLBACK lpCallback, const char* className, void* lpData);
	HRESULT LoadTable(CBGame* Game, CBPersistMgr* PersistMgr);
	HRESULT SaveTable(CBGame* Game, CBPersistMgr* PersistMgr, bool quickSave);
	HRESULT LoadInstances(CBGame* Game, CBPersistMgr* PersistMgr);
	HRESULT SaveInstances(CBGame* Game, CBPersistMgr* PersistMgr, bool quickSave);
	void* IDToPointer(int classID, int instanceID);
	bool GetPointerID(void* pointer, int* classID, int* instanceID);
	bool RegisterClass(CSysClass* classObj);
	bool UnregisterClass(CSysClass* classObj);
	bool RegisterInstance(const char* className, void* instance);
	bool UnregisterInstance(const char* className, void* instance);
	void DumpClasses(FILE* stream);
	int GetNextID();
	void AddInstanceToTable(CSysInstance* instance, void* pointer);

	CSysClassRegistry();
	virtual ~CSysClassRegistry();

	bool m_Disabled;
	int m_Count;
	
	typedef std::set<CSysClass*> Classes;
	Classes m_Classes;

	typedef std::map<AnsiString, CSysClass*> NameMap;
	NameMap m_NameMap;

	typedef std::map<int, CSysClass*> IdMap;
	IdMap m_IdMap;

	typedef std::map<void*, CSysInstance*> InstanceMap;
	InstanceMap m_InstanceMap;

	typedef std::map<int, CSysInstance*> SavedInstanceMap;
	SavedInstanceMap m_SavedInstanceMap;

};

#endif
