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

#ifndef __WmeBSurfaceStorage_H__
#define __WmeBSurfaceStorage_H__


#include "coll_templ.h"

class CBSurfaceStorage : public CBBase
{
public:
	DWORD m_LastCleanupTime;
	HRESULT InitLoop();
	HRESULT SortSurfaces();
	static int SurfaceSortCB(const void* arg1, const void* arg2);
	HRESULT Cleanup(bool Warn=false);
	//DECLARE_PERSISTENT(CBSurfaceStorage, CBBase);

	HRESULT RestoreAll();
	CBSurface* AddSurface(char* Filename, bool default_ck=true, BYTE ck_red=0, BYTE ck_green=0, BYTE ck_blue=0, int LifeTime=-1, bool KeepLoaded=false);
	HRESULT RemoveSurface(CBSurface* surface);
	CBSurfaceStorage(CBGame* inGame);
	virtual ~CBSurfaceStorage();

	CBArray<CBSurface*, CBSurface*> m_Surfaces;
};

#endif
