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

#ifndef __WmeAdGame_H__
#define __WmeAdGame_H__

#include "BGame.h"

class CAdGame : public CBGame  
{
public:
	virtual HRESULT OnScriptShutdown(CScScript* Script);

	virtual HRESULT OnMouseLeftDown();
	virtual HRESULT OnMouseLeftUp();
	virtual HRESULT OnMouseLeftDblClick();
	virtual HRESULT OnMouseRightDown();
	virtual HRESULT OnMouseRightUp();

	virtual HRESULT DisplayDebugInfo();


	virtual HRESULT InitAfterLoad();
	static void AfterLoadScene(void* Scene, void* Data);

	bool m_SmartItemCursor;

	CBArray<char*, char*> m_SpeechDirs;
	HRESULT AddSpeechDir(char* Dir);
	HRESULT RemoveSpeechDir(char* Dir);
	char* FindSpeechFile(char* StringID);

	HRESULT DeleteItem(CAdItem* Item);
	char* m_ItemsFile;
	bool m_TempDisableSaveState;
	virtual HRESULT ResetContent();
	HRESULT AddItem(CAdItem *Item);
	CAdItem* GetItemByName(char *Name);
	CBArray<CAdItem*, CAdItem*> m_Items;
	CAdObject* m_InventoryOwner;
	bool IsItemTaken(char* ItemName);
	HRESULT RegisterInventory(CAdInventory* Inv);
	HRESULT UnregisterInventory(CAdInventory* Inv);

	CAdObject* m_InvObject;
	CBArray<CAdInventory*, CAdInventory*> m_Inventories;
	virtual HRESULT DisplayContent(bool Update=true, bool DisplayAll=false);
	char* m_DebugStartupScene;
	char* m_StartupScene;
	bool m_InitialScene;
	bool GameResponseUsed(int ID);
	HRESULT AddGameResponse(int ID);
	HRESULT ResetResponse(int ID);

	bool BranchResponseUsed(int ID);
	HRESULT AddBranchResponse(int ID);
	HRESULT ClearBranchResponses(char* Name);
	HRESULT StartDlgBranch(const char* BranchName, const char* ScriptName, const char* EventName);
	HRESULT EndDlgBranch(const char* BranchName, const char* ScriptName, const char* EventName);
	virtual HRESULT WindowLoadHook(CUIWindow *Win, char **Buf, char **Params);
	virtual HRESULT WindowScriptMethodHook(CUIWindow* Win, CScScript* Script, CScStack* Stack, char* Name);

	CAdSceneState* GetSceneState(char* Filename, bool Saving);
	CBViewport* m_SceneViewport;
	int m_TexItemLifeTime;
	int m_TexWalkLifeTime;
	int m_TexStandLifeTime;
	int m_TexTalkLifeTime;

	TTalkSkipButton m_TalkSkipButton;

	virtual HRESULT GetVersion(BYTE* VerMajor, BYTE* VerMinor, BYTE* ExtMajor, BYTE* ExtMinor);
	HRESULT ScheduleChangeScene(char *Filename, bool FadeIn);
	char* m_ScheduledScene;
	bool m_ScheduledFadeIn;
	void SetPrevSceneName(char* Name);
	void SetPrevSceneFilename(char* Name);
	char* m_PrevSceneName;
	char* m_PrevSceneFilename;
	virtual HRESULT LoadGame(char* Filename);
	CAdItem* m_SelectedItem;
	HRESULT Cleanup();
	DECLARE_PERSISTENT(CAdGame, CBGame);

	void FinishSentences();
	HRESULT ShowCursor();
	TGameStateEx m_StateEx;
	CAdResponseBox* m_ResponseBox;
	CAdInventoryBox* m_InventoryBox;
	HRESULT DisplaySentences(bool Frozen);
	void AddSentence(CAdSentence* Sentence);
	HRESULT ChangeScene(char* Filename, bool FadeIn);
	HRESULT RemoveObject(CAdObject* Object);
	HRESULT AddObject(CAdObject* Object);
	CAdScene* m_Scene;
	HRESULT InitLoop();
	CAdGame();
	virtual ~CAdGame();
	CBArray<CAdObject*, CAdObject*> m_Objects;
	CBArray<CAdSentence*, CAdSentence*> m_Sentences;
	
	CBArray<CAdSceneState*, CAdSceneState*> m_SceneStates;
	CBArray<char*, char*> m_DlgPendingBranches;

	CBArray<CAdResponseContext*, CAdResponseContext*> m_ResponsesBranch;
	CBArray<CAdResponseContext*, CAdResponseContext*> m_ResponsesGame;

	virtual HRESULT LoadFile(const char * Filename);
	virtual HRESULT LoadBuffer(BYTE * Buffer, bool Complete=true);

	HRESULT LoadItemsFile(char * Filename, bool Merge=false);
	HRESULT LoadItemsBuffer(BYTE * Buffer, bool Merge=false);


	virtual void PublishNatives();
	virtual HRESULT ExternalCall(CScScript* Script, CScStack* Stack, CScStack* ThisStack, char* Name);

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);	
	bool ValidMouse();
};

#endif
