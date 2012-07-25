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
#include "BTransitionMgr.h"


//////////////////////////////////////////////////////////////////////////
CBTransitionMgr::CBTransitionMgr(CBGame* inGame):CBBase(inGame)
{
	m_State = TRANS_MGR_READY;
	m_Type = TRANSITION_NONE;
	m_OrigInteractive = false;
	m_PreserveInteractive = false;
	m_LastTime = 0;
	m_Started = false;
}



//////////////////////////////////////////////////////////////////////////
CBTransitionMgr::~CBTransitionMgr()
{

}


//////////////////////////////////////////////////////////////////////////
bool CBTransitionMgr::IsReady()
{
	return (m_State==TRANS_MGR_READY);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBTransitionMgr::Start(TTransitionType Type, bool NonInteractive)
{
	if(m_State != TRANS_MGR_READY) return S_OK;

	if(Type==TRANSITION_NONE || Type>=NUM_TRANSITION_TYPES){
		m_State = TRANS_MGR_READY;
		return S_OK;
	}

	if(NonInteractive){
		m_PreserveInteractive = true;
		m_OrigInteractive = Game->m_Interactive;
		Game->m_Interactive = false;
	}
	else m_PreserveInteractive;

	m_Type = Type;	
	m_State = TRANS_MGR_RUNNING;
	m_Started = false;

	return S_OK;
}

#define FADE_DURATION 200

//////////////////////////////////////////////////////////////////////////
HRESULT CBTransitionMgr::Update()
{
	if(IsReady()) return S_OK;

	if(!m_Started){
		m_Started = true;
		m_LastTime = CBPlatform::GetTime();
	}

	switch(m_Type){
		case TRANSITION_NONE:
			m_State = TRANS_MGR_READY;
		break;

		case TRANSITION_FADE_OUT:
		{		
			DWORD time = CBPlatform::GetTime() - m_LastTime;
			int Alpha = 255 - (float)time / (float)FADE_DURATION * 255;
			Alpha = min(255, max(Alpha, 0));
			Game->m_Renderer->Fade((WORD)Alpha);
			
			if(time > FADE_DURATION) m_State = TRANS_MGR_READY;
		}	
		break;

		case TRANSITION_FADE_IN:
		{		
			DWORD time = CBPlatform::GetTime() - m_LastTime;
			int Alpha = (float)time / (float)FADE_DURATION * 255;
			Alpha = min(255, max(Alpha, 0));
			Game->m_Renderer->Fade((WORD)Alpha);
			
			if(time > FADE_DURATION) m_State = TRANS_MGR_READY;
		}	
		break;

	}

	if(IsReady()){
		if(m_PreserveInteractive) Game->m_Interactive = m_OrigInteractive;
	}
	return S_OK;
}
