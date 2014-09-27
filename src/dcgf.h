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

#ifndef _DCGF_H_
#define _DCGF_H_


//////////////////////////////////////////////////////////////////////////
#define DCGF_VER_MAJOR 1
#define DCGF_VER_MINOR 0
#define DCGF_VER_BUILD 6
#define DCGF_VER_SUFFIX "rapaki"
#define DCGF_VER_BETA true

#define DCGF_NAME "WME Lite"
#define DCGF_MAGIC 0xDEC0ADDE

// minimal saved game version we support
#define SAVEGAME_VER_MAJOR 1
#define SAVEGAME_VER_MINOR 0
#define SAVEGAME_VER_BUILD 0
//////////////////////////////////////////////////////////////////////////

#define COMPRESSED_FILE_MAGIC 0x504D435A // ZCMP

#ifdef _MSC_VER
#	pragma warning (disable: 4786) // symbols longer than 255 chars (STL)
#	pragma warning (disable: 4355) // this in base member initialization
#	pragma warning (disable: 4018) // signed/unsigned match
#	pragma warning (disable: 4244) // conversion from float to int
#	pragma warning (disable: 4996) // CRT deprecate
#	pragma warning (disable: 4799) // No EMMS at end of function ...
#	pragma warning (disable: 4701) // No EMMS at end of function ...
#	pragma warning (disable: 4200) // nonstandard extension used : zero-sized array in struct/union
#endif



#include "PlatformSDL.h"

#include <stdio.h>

#include <assert.h>


#ifdef GetClassName
	#undef GetClassName
#endif


#include "SDL.h"
#include "dctypes.h"
#include "dcpackage.h"
#include "utils.h"


#include "coll_templ.h"

class CBPersistMgr;

// persistence support
typedef void* (WINAPI *PERSISTBUILD)(void);
typedef HRESULT (WINAPI *PERSISTLOAD)(void*, CBPersistMgr*);

#define DECLARE_PERSISTENT(class_name, parent_class)\
	static const char m_ClassName[];\
	static void* WINAPI PersistBuild(void);\
	virtual const char* GetClassName();\
	static HRESULT WINAPI PersistLoad(void* Instance, CBPersistMgr* PersistMgr);\
	class_name(TDynamicConstructor p1, TDynamicConstructor p2):parent_class(p1, p2){ /*memset(this, 0, sizeof(class_name));*/ };\
	virtual HRESULT Persist(CBPersistMgr* PersistMgr);\
	void* operator new (size_t size);\
	void operator delete(void* p);\


#define IMPLEMENT_PERSISTENT(class_name, persistent_class)\
	const char class_name::m_ClassName[] = #class_name;\
	void* class_name::PersistBuild(){\
		return ::new class_name(DYNAMIC_CONSTRUCTOR, DYNAMIC_CONSTRUCTOR);\
	}\
	\
	HRESULT class_name::PersistLoad(void* Instance, CBPersistMgr* PersistMgr){\
		return ((class_name*)Instance)->Persist(PersistMgr);\
	}\
	\
	const char* class_name::GetClassName(){\
		return #class_name;\
	}\
	\
	CSysClass Register##class_name(class_name::m_ClassName, class_name::PersistBuild, class_name::PersistLoad, persistent_class);\
	\
	void* class_name::operator new (size_t size){\
		void* ret = ::operator new(size);\
		CSysClassRegistry::GetInstance()->RegisterInstance(#class_name, ret);\
		return ret;\
	}\
	\
	void class_name::operator delete (void* p){\
		CSysClassRegistry::GetInstance()->UnregisterInstance(#class_name, p);\
		::operator delete(p);\
	}\

#define TMEMBER(member_name) #member_name, &member_name
#define TMEMBER_INT(member_name) #member_name, (int*)&member_name

typedef void (*SYS_INSTANCE_CALLBACK)(void* Instance, void* Data);


// classes/instances registry
#include "SysInstance.h"
#include "SysClass.h"
#include "SysClassRegistry.h"

// base classes
#include "BBase.h"
#include "BNamedObject.h"
#include "BParser.h"
#include "BDynBuffer.h"
#include "BStringTable.h"
#include "BRegistry.h"
#include "BPersistMgr.h"
#include "BScriptable.h"
#include "BScriptHolder.h"
#include "BFileEntry.h"
#include "BPackage.h"
#include "BFile.h"
#include "BResourceFile.h"
#include "BSaveThumbFile.h"
#include "BDiskFile.h"
#include "BPkgFile.h"
#include "BFileManager.h"
#include "BEvent.h"
#include "BPoint.h"
#include "BObject.h"
#include "BViewport.h"
#include "BRegion.h"
#include "BSurface.h"
#include "BSurfaceStorage.h"
#include "BSoundBuffer.h"
#include "BSoundMgr.h"
#include "BSound.h"
#include "BSubFrame.h"
#include "BFrame.h"
#include "BActiveRect.h"
#include "BSprite.h"
#include "BFontStorage.h"
#include "BFont.h"
#include "BFontBitmap.h"
#include "BFontTT.h"
#include "BRenderer.h"
#include "BImage.h"
#include "BQuickMsg.h"
#include "BTransitionMgr.h"
#include "BKeyboardState.h"
#include "BFader.h"
#include "SXMath.h"
#include "SXMemBuffer.h"
#include "SXFile.h"
#include "PartEmitter.h"

// UI classes
#include "UITiledImage.h"
#include "UIObject.h"
#include "UIButton.h"
#include "UIEdit.h"
#include "UIText.h"
#include "UIWindow.h"

// scripting classes
#include "dcscript.h"
#include "ScValue.h"
#include "ScStack.h"
#include "ScScript.h"
#include "ScEngine.h"

// video classes
#include "VidManager.h"
#include "VidTheoraPlayer.h"

#include "AdTypes.h"

#include "AdSpriteSet.h"
#include "AdTalkNode.h"
#include "AdTalkDef.h"
#include "AdResponseContext.h"
#include "AdNodeState.h"
#include "AdSceneState.h"
#include "AdResponse.h"
#include "AdResponseBox.h"
#include "AdPathPoint.h"
#include "AdPath.h"
#include "AdSentence.h"
#include "AdInventory.h"
#include "AdObject.h"
#include "AdTalkHolder.h"
#include "AdItem.h"
#include "AdActorDir.h"
#include "AdActor.h"
#include "AdEntity.h"
#include "AdSceneNode.h"
#include "AdRegion.h"
#include "AdLayer.h"
#include "AdWaypointGroup.h"
#include "AdScaleLevel.h"
#include "AdRotLevel.h"
#include "AdInventoryBox.h"
#include "AdScene.h"
#include "AdGame.h"
#include "UIEntity.h"


// big boss
#include "BGame.h"

#define PI ((float) 3.141592653589793f)

// macros
#define RELEASE(obj) if(obj) { obj->Release(); obj = NULL; } else (void)0
#define SAFE_DELETE(obj)  if(obj) { delete obj; obj = NULL; } else (void)0
#define SAFE_DELETE_ARRAY(obj)  if(obj) { delete [] obj; obj = NULL; } else (void)0
#define DRGBA(r,g,b,a) ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define DegToRad(_val) (_val*PI*(1.0f/180.0f))
#define RadToDeg(_val) (_val*(180/PI))

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define D3DCOLGetB(rgb)	 ((BYTE)(rgb))
#define D3DCOLGetG(rgb)  ((BYTE)(((WORD)(rgb)) >> 8))
#define D3DCOLGetR(rgb)  ((BYTE)((rgb)>>16))
#define D3DCOLGetA(rgb)  ((BYTE)((rgb)>>24))


#endif // _DCGF_H_
