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
#include "BFader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_PERSISTENT(CBFader, false);

//////////////////////////////////////////////////////////////////////////
CBFader::CBFader(CBGame* inGame):CBObject(inGame)
{
	m_Active = false;
	m_Red = m_Green = m_Blue = 0;
	m_CurrentAlpha = 0x00;
	m_SourceAlpha = 0;
	m_TargetAlpha = 0;
	m_Duration = 1000;
	m_StartTime = 0;
	m_System = false;
}


//////////////////////////////////////////////////////////////////////////
CBFader::~CBFader()
{

}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::Update()
{
	if(!m_Active) return S_OK;

	int AlphaDelta = m_TargetAlpha - m_SourceAlpha;

	DWORD time;
	
	if(m_System) time = CBPlatform::GetTime() - m_StartTime;
	else time = Game->m_Timer - m_StartTime;

	if(time >= m_Duration) m_CurrentAlpha = m_TargetAlpha;
	else{
		m_CurrentAlpha = m_SourceAlpha + (float)time / (float)m_Duration * AlphaDelta;
	}
	m_CurrentAlpha = MIN(255, max(m_CurrentAlpha, (BYTE)0));
	
	m_Ready = time >= m_Duration;
	if(m_Ready && m_CurrentAlpha==0x00) m_Active = false;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::Display()
{
	if(!m_Active) return S_OK;

	if(m_CurrentAlpha>0x00) return Game->m_Renderer->FadeToColor(DRGBA(m_Red, m_Green, m_Blue, m_CurrentAlpha));
	else return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::Deactivate()
{
	m_Active = false;
	m_Ready = true;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::FadeIn(DWORD SourceColor, DWORD Duration, bool System)
{
	m_Ready = false;
	m_Active = true;

	m_Red   = D3DCOLGetR(SourceColor);
	m_Green = D3DCOLGetG(SourceColor);
	m_Blue  = D3DCOLGetB(SourceColor);

	m_SourceAlpha = D3DCOLGetA(SourceColor);
	m_TargetAlpha = 0;

	m_Duration = Duration;
	m_System = System;
	
	if(m_System) m_StartTime = CBPlatform::GetTime();
	else m_StartTime = Game->m_Timer;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::FadeOut(DWORD TargetColor, DWORD Duration, bool System)
{
	m_Ready = false;
	m_Active = true;

	m_Red   = D3DCOLGetR(TargetColor);
	m_Green = D3DCOLGetG(TargetColor);
	m_Blue  = D3DCOLGetB(TargetColor);

	//m_SourceAlpha = 0;
	m_SourceAlpha = m_CurrentAlpha;
	m_TargetAlpha = D3DCOLGetA(TargetColor);

	m_Duration = Duration;
	m_System = System;

	if(m_System) m_StartTime = CBPlatform::GetTime();
	else m_StartTime = Game->m_Timer;


	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
DWORD CBFader::GetCurrentColor()
{
	return DRGBA(m_Red, m_Green, m_Blue, m_CurrentAlpha);
}



//////////////////////////////////////////////////////////////////////////
HRESULT CBFader::Persist(CBPersistMgr *PersistMgr)
{
	CBObject::Persist(PersistMgr);

	PersistMgr->Transfer(TMEMBER(m_Active));
	PersistMgr->Transfer(TMEMBER(m_Blue));
	PersistMgr->Transfer(TMEMBER(m_CurrentAlpha));
	PersistMgr->Transfer(TMEMBER(m_Duration));
	PersistMgr->Transfer(TMEMBER(m_Green));
	PersistMgr->Transfer(TMEMBER(m_Red));
	PersistMgr->Transfer(TMEMBER(m_SourceAlpha));
	PersistMgr->Transfer(TMEMBER(m_StartTime));
	PersistMgr->Transfer(TMEMBER(m_TargetAlpha));
	PersistMgr->Transfer(TMEMBER(m_System));

	if(m_System && !PersistMgr->m_Saving) m_StartTime = 0;

	return S_OK;
}
