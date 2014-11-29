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
#include "AdSentence.h"
#include "PathUtil.h"


IMPLEMENT_PERSISTENT(CAdSentence, false);

//////////////////////////////////////////////////////////////////////////
CAdSentence::CAdSentence(CBGame* inGame):CBBase(inGame)
{
	m_Text = NULL;
	m_Stances = NULL;
	m_TempStance = NULL;
	
	m_Duration = 0;
	m_StartTime = 0;
	m_CurrentStance = 0;

	m_Font = NULL;

	m_Pos.x = m_Pos.y = 0;
	m_Width = Game->m_Renderer->m_Width;

	m_Align = (TTextAlign)TAL_CENTER;

	m_Sound = NULL;
	m_SoundStarted = false;

	m_TalkDef = NULL;
	m_CurrentSprite = NULL;
	m_CurrentSkelAnim = NULL;
	m_FixedPos = false;
	m_Freezable = true;
}


//////////////////////////////////////////////////////////////////////////
CAdSentence::~CAdSentence()
{
	SAFE_DELETE(m_Sound);
	SAFE_DELETE_ARRAY(m_Text);
	SAFE_DELETE_ARRAY(m_Stances);
	SAFE_DELETE_ARRAY(m_TempStance);
	SAFE_DELETE(m_TalkDef);
	
	m_CurrentSprite = NULL; // ref only
	m_CurrentSkelAnim = NULL;
	m_Font = NULL; // ref only
}


//////////////////////////////////////////////////////////////////////////
void CAdSentence::SetText(char *Text)
{
	if(m_Text) delete [] m_Text;
	m_Text = new char[strlen(Text)+1];
	if(m_Text) strcpy(m_Text, Text);
}


//////////////////////////////////////////////////////////////////////////
void CAdSentence::SetStances(char *Stances)
{
	if(m_Stances) delete [] m_Stances;
	if(Stances)
	{
		m_Stances = new char[strlen(Stances)+1];
		if(m_Stances) strcpy(m_Stances, Stances);
	}
	else m_Stances = NULL;
}


//////////////////////////////////////////////////////////////////////////
char* CAdSentence::GetCurrentStance()
{
	return GetStance(m_CurrentStance);
}


//////////////////////////////////////////////////////////////////////////
char* CAdSentence::GetNextStance()
{
	m_CurrentStance++;
	return GetStance(m_CurrentStance);
}


//////////////////////////////////////////////////////////////////////////
char* CAdSentence::GetStance(int Stance)
{
	if(m_Stances==NULL) return NULL;

	if(m_TempStance) delete [] m_TempStance;
	m_TempStance = NULL;

	char* start;
	char* curr;
	int pos;

	if(Stance==0) start = m_Stances;
	else{
		pos=0;
		start = NULL;
		curr = m_Stances;
		while(pos < Stance){
			if(*curr=='\0') break;
			if(*curr==',') pos++;
			curr++;
		}
		if(pos==Stance) start = curr;		
	}

	if(start==NULL) return NULL;

	while(*start==' ' && *start!=',' && *start!='\0') start++;

	curr = start;
	while(*curr!='\0' && *curr!=',') curr++;

	while(curr > start && *(curr-1)==' ') curr--;

	m_TempStance = new char [curr-start+1];
	if(m_TempStance){
		m_TempStance[curr-start] = '\0';
		strncpy(m_TempStance, start, curr-start);
	}

	return m_TempStance;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::Display()
{
	if(!m_Font || !m_Text) return E_FAIL;

	if(m_Sound && !m_SoundStarted){
		m_Sound->Play();
		m_SoundStarted = true;
	}

	if(Game->m_Subtitles){
		int x = m_Pos.x;
		int y = m_Pos.y;

		if(!m_FixedPos){
			x = x - ((CAdGame*)Game)->m_Scene->GetOffsetLeft();
			y = y - ((CAdGame*)Game)->m_Scene->GetOffsetTop();
		}


		x = max(x, 0);
		x = min(x, Game->m_Renderer->m_Width - m_Width);
		y = max(y, 0);

		m_Font->DrawText((BYTE*)m_Text, x, y, m_Width, m_Align);
	}

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
void CAdSentence::SetSound(CBSound *Sound)
{
	if(!Sound) return;
	SAFE_DELETE(m_Sound);
	m_Sound = Sound;
	m_SoundStarted = false;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::Finish()
{
	if(m_Sound) m_Sound->Stop();
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::Persist(CBPersistMgr* PersistMgr){

	PersistMgr->Transfer(TMEMBER(Game));

	PersistMgr->Transfer(TMEMBER_INT(m_Align));
	PersistMgr->Transfer(TMEMBER(m_CurrentStance));
	PersistMgr->Transfer(TMEMBER(m_CurrentSprite));
	PersistMgr->Transfer(TMEMBER(m_CurrentSkelAnim));
	PersistMgr->Transfer(TMEMBER(m_Duration));
	PersistMgr->Transfer(TMEMBER(m_Font));
	PersistMgr->Transfer(TMEMBER(m_Pos));
	PersistMgr->Transfer(TMEMBER(m_Sound));
	PersistMgr->Transfer(TMEMBER(m_SoundStarted));
	PersistMgr->Transfer(TMEMBER(m_Stances));
	PersistMgr->Transfer(TMEMBER(m_StartTime));
	PersistMgr->Transfer(TMEMBER(m_TalkDef));
	PersistMgr->Transfer(TMEMBER(m_TempStance));
	PersistMgr->Transfer(TMEMBER(m_Text));
	PersistMgr->Transfer(TMEMBER(m_Width));
	PersistMgr->Transfer(TMEMBER(m_FixedPos));
	PersistMgr->Transfer(TMEMBER(m_Freezable));

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::SetupTalkFile(char* SoundFilename)
{
	SAFE_DELETE(m_TalkDef);
	m_CurrentSprite = NULL;

	if(!SoundFilename) return S_OK;
	
	
	AnsiString path = PathUtil::GetDirectoryName(SoundFilename);
	AnsiString name = PathUtil::GetFileNameWithoutExtension(SoundFilename);

	AnsiString talkDefFileName = PathUtil::Combine(path, name + ".talk");

	CBFile* file = Game->m_FileManager->OpenFile(talkDefFileName.c_str());
	if (file)
	{
		Game->m_FileManager->CloseFile(file);
	}
	else return S_OK; // no talk def file found


	m_TalkDef = new CAdTalkDef(Game);
	if (!m_TalkDef || FAILED(m_TalkDef->LoadFile(talkDefFileName.c_str())))
	{
		SAFE_DELETE(m_TalkDef);
		return E_FAIL;
	}
	//Game->LOG(0, "Using .talk file: %s", TalkDefFile);
	
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CAdSentence::Update(TDirection Dir)
{
	if(!m_TalkDef) return S_OK;

	DWORD CurrentTime;
	// if sound is available, synchronize with sound, otherwise use timer
	
	/*
	if(m_Sound) CurrentTime = m_Sound->GetPositionTime();
	else CurrentTime = Game->m_Timer - m_StartTime;
	*/
	CurrentTime = Game->m_Timer - m_StartTime;

	bool TalkNodeFound = false;
	for(int i=0; i<m_TalkDef->m_Nodes.GetSize(); i++){
		if(m_TalkDef->m_Nodes[i]->IsInTimeInterval(CurrentTime, Dir)){
			TalkNodeFound = true;

			CBSprite* NewSprite = m_TalkDef->m_Nodes[i]->GetSprite(Dir);
			if(NewSprite != m_CurrentSprite) NewSprite->Reset();
			m_CurrentSprite = NewSprite;

			if(!m_TalkDef->m_Nodes[i]->m_PlayToEnd) break;
		}
	}


	// no talk node, try to use default sprite instead (if any)
	if(!TalkNodeFound){
		CBSprite* NewSprite = m_TalkDef->GetDefaultSprite(Dir);
		if(NewSprite){
			if(NewSprite != m_CurrentSprite) NewSprite->Reset();
			m_CurrentSprite = NewSprite;
		}
		else m_CurrentSprite = NULL;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CAdSentence::CanSkip()
{
	// prevent accidental sentence skipping (TODO make configurable)
	return (Game->m_Timer - m_StartTime) > 300;
}
