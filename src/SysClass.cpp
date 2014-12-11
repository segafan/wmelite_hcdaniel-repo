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
#include "SysClass.h"



//////////////////////////////////////////////////////////////////////////
CSysClass::CSysClass(const AnsiString& name, PERSISTBUILD build, PERSISTLOAD load, bool persistent_class)
{
	m_Name = name;

	m_Build = build;
	m_Load = load;
	m_Next = NULL;
	m_SavedID = -1;
	m_Persistent = persistent_class;
	m_NumInst = 0;

	CSysClassRegistry::GetInstance()->RegisterClass(this);
}


//////////////////////////////////////////////////////////////////////////
CSysClass::~CSysClass()
{
	CSysClassRegistry::GetInstance()->UnregisterClass(this);
	RemoveAllInstances();
}

//////////////////////////////////////////////////////////////////////////
bool CSysClass::RemoveAllInstances()
{
	Instances::iterator it;
	for (it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		delete (*it);
	}
	m_Instances.clear();
	m_InstanceMap.clear();

	return true;
}

//////////////////////////////////////////////////////////////////////////
CSysInstance* CSysClass::AddInstance(void* instance, int id, int savedId)
{
	CSysInstance* inst = new CSysInstance(instance, id, this);	
	inst->SetSavedID(savedId);
	m_Instances.insert(inst);

	m_InstanceMap[instance] = inst;

	CSysClassRegistry::GetInstance()->AddInstanceToTable(inst, instance);

	return inst;
}


//////////////////////////////////////////////////////////////////////////
bool CSysClass::RemoveInstance(void* instance)
{
	InstanceMap::iterator mapIt = m_InstanceMap.find(instance);
	if (mapIt == m_InstanceMap.end()) return false;

	Instances::iterator it = m_Instances.find((*mapIt).second);
	if (it != m_Instances.end())
	{
		delete (*it);
		m_Instances.erase(it);
	}

	m_InstanceMap.erase(mapIt);

	return false;
}

//////////////////////////////////////////////////////////////////////////
int CSysClass::GetInstanceID(void* pointer)
{	
	InstanceMap::iterator mapIt = m_InstanceMap.find(pointer);
	if (mapIt == m_InstanceMap.end()) return -1;
	else return (*mapIt).second->GetID();
}

//////////////////////////////////////////////////////////////////////////
void* CSysClass::IDToPointer(int savedID)
{
	//slow
	Instances::iterator it;
	for (it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		if ((*it)->GetSavedID() == savedID) return (*it)->GetInstance();
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
int CSysClass::GetNumInstances()
{
	return m_Instances.size();
}

//////////////////////////////////////////////////////////////////////////
void CSysClass::Dump(FILE* stream)
{
	fprintf(stream, "%03d %c %-20s instances: %d\n", m_ID, m_Persistent?'p':' ', m_Name.c_str(), GetNumInstances());
}

//////////////////////////////////////////////////////////////////////////
void CSysClass::SaveTable(CBGame* Game, CBPersistMgr* PersistMgr)
{
	PersistMgr->PutString(m_Name.c_str());
	PersistMgr->PutDWORD(m_ID);
	PersistMgr->PutDWORD(m_Instances.size());

	Instances::iterator it;
	for (it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		PersistMgr->PutDWORD((*it)->GetID());
	}
}

//////////////////////////////////////////////////////////////////////////
void CSysClass::LoadTable(CBGame* Game, CBPersistMgr* PersistMgr)
{
	m_SavedID = PersistMgr->GetDWORD();
	int numInstances = PersistMgr->GetDWORD();

	for (int i = 0; i < numInstances; i++)
	{
		if (m_Persistent)
		{
			int instId = PersistMgr->GetDWORD();

			if (i > 0)
			{
				Game->LOG(0, "Warning: attempting to load multiple instances of persistent class %s (%d)", m_Name.c_str(), numInstances);
				continue;
			}

			Instances::iterator it = m_Instances.begin();
			if (it != m_Instances.end())
			{
				(*it)->SetSavedID(instId);
				CSysClassRegistry::GetInstance()->AddInstanceToTable((*it), (*it)->GetInstance());
			}
			else Game->LOG(0, "Warning: instance %d of persistent class %s not found", i, m_Name.c_str());
		}
		// normal instances, create empty objects
		else
		{
			void* emptyObject = m_Build();
			AddInstance(emptyObject, CSysClassRegistry::GetInstance()->GetNextID(), PersistMgr->GetDWORD());
		}

	}
}

//////////////////////////////////////////////////////////////////////////
void CSysClass::SaveInstances(CBGame* Game, CBPersistMgr* PersistMgr)
{
	Instances::iterator it;
	for (it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		// write instace header
		PersistMgr->PutDWORD(m_ID);
		PersistMgr->PutDWORD((*it)->GetID());

		m_Load((*it)->GetInstance(), PersistMgr);
	}
}

//////////////////////////////////////////////////////////////////////////
void CSysClass::LoadInstance(void* instance, CBPersistMgr* PersistMgr)
{
	m_Load(instance, PersistMgr);
}


//////////////////////////////////////////////////////////////////////////
void CSysClass::ResetSavedIDs()
{
	Instances::iterator it;
	for (it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		(*it)->SetSavedID(-1);
	}
}

//////////////////////////////////////////////////////////////////////////
void CSysClass::InstanceCallback(SYS_INSTANCE_CALLBACK lpCallback, void* lpData)
{
	Instances::iterator it;
	for (it = m_Instances.begin(); it != m_Instances.end(); ++it)
	{
		lpCallback((*it)->GetInstance(), lpData);
	}
}
