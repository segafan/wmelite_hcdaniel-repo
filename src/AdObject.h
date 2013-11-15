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

#ifndef __WmeAdObject_H__
#define __WmeAdObject_H__


class CAdWaypointGroup;
class CAdRegion;

#define MAX_NUM_REGIONS 10

class CAdObject : public CBObject
{
public:
	CPartEmitter* m_PartEmitter;
	virtual CPartEmitter* CreateParticleEmitter(bool FollowParent=false, int OffsetX=0, int OffsetY=0);
	virtual HRESULT UpdatePartEmitter();
	bool m_PartFollowParent;
	int m_PartOffsetX;
	int m_PartOffsetY;

	HRESULT InvalidateCurrRegions();
	bool m_SubtitlesModRelative;
	bool m_SubtitlesModXCenter;
	int m_SubtitlesModX;
	int m_SubtitlesModY;
	int m_SubtitlesWidth;
	CAdRegion* m_StickRegion;
	bool m_SceneIndependent;
	bool m_IgnoreItems;
	HRESULT UpdateBlockRegion();
	bool m_ForcedTalkAnimUsed;
	char* m_ForcedTalkAnimName;
	virtual bool GetExtendedFlag(char* FlagName);
	virtual HRESULT ResetSoundPan();
	virtual HRESULT UpdateSounds();
	HRESULT Reset();
	DECLARE_PERSISTENT(CAdObject, CBObject);
	virtual void Talk(char* Text, char* Sound=NULL, DWORD Duration=0, char* Stances=NULL, TTextAlign Align=TAL_CENTER);
	virtual int GetHeight();
	CAdSentence* m_Sentence;
	HRESULT SetFont(char* Filename);
	virtual HRESULT Update();
	virtual HRESULT Display();
	bool m_Drawn;
	bool m_Active;
	virtual HRESULT PlayAnim(char* Filename);
	CBSprite* m_AnimSprite;
	CBSprite* m_CurrentSprite;
	TObjectState m_State;
	TObjectState m_NextState;
	TObjectType m_Type;
	CAdObject(CBGame* inGame);
	virtual ~CAdObject();
	CBFont* m_Font;
	CBSprite* m_TempSprite2;
	CBRegion* m_BlockRegion;
	CAdWaypointGroup* m_WptGroup;
	CBRegion* m_CurrentBlockRegion;
	CAdWaypointGroup* m_CurrentWptGroup;
	CAdInventory* GetInventory();

	virtual HRESULT SaveAsText(CBDynBuffer *Buffer, int Indent);
	
	virtual HRESULT AfterMove();
	CAdRegion* m_CurrentRegions[MAX_NUM_REGIONS];	

	// scripting interface
	virtual CScValue* ScGetProperty(char* Name);
	virtual HRESULT ScSetProperty(char *Name, CScValue *Value);
	virtual HRESULT ScCallMethod(CScScript* Script, CScStack *Stack, CScStack *ThisStack, char *Name);
	virtual char* ScToString();

	CBArray<CAdObject*, CAdObject*> m_AttachmentsPre;
	CBArray<CAdObject*, CAdObject*> m_AttachmentsPost;

	HRESULT UpdateSpriteAttachments();
	HRESULT DisplaySpriteAttachments(bool PreDisplay);	
	CAdObject* m_RegisterAlias;
private:
	HRESULT DisplaySpriteAttachment(CAdObject* Attachment);
	CAdInventory* m_Inventory;

protected:
	HRESULT GetScale(float* ScaleX, float* ScaleY);
};

#endif
