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

#ifndef __WmeAdSpriteSet_H__
#define __WmeAdSpriteSet_H__


#include "BObject.h"
#include "BSprite.h"	// Added by ClassView

class CAdSpriteSet : public CBObject  
{
public:
	bool ContainsSprite(CBSprite* Sprite);
	virtual HRESULT SaveAsText(CBDynBuffer* Buffer, int Indent=0);
	CBSprite* GetSprite(TDirection Direction);
	DECLARE_PERSISTENT(CAdSpriteSet, CBObject);
	CBObject* m_Owner;	
	CAdSpriteSet(CBGame* inGame, CBObject* Owner=NULL);
	virtual ~CAdSpriteSet();
	HRESULT LoadFile(char* Filename, int LifeTime=-1, TSpriteCacheType CacheType=CACHE_ALL);
	HRESULT LoadBuffer(BYTE* Buffer, bool Complete=true, int LifeTime=-1, TSpriteCacheType CacheType=CACHE_ALL);
	CBSprite* m_Sprites[NUM_DIRECTIONS];
};

#endif
