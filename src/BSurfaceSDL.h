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

#ifndef __WmeBSurfaceSDL_H__
#define __WmeBSurfaceSDL_H__


#include "BSurface.h"
#include "SDL.h"
#include "FreeImage.h"


class CBSurfaceSDL : public CBSurface
{
public:
	CBSurfaceSDL(CBGame* inGame);
	~CBSurfaceSDL();

	HRESULT Create(char* Filename, bool default_ck, BYTE ck_red, BYTE ck_green, BYTE ck_blue, int LifeTime=-1, bool KeepLoaded=false);
	HRESULT Create(int Width, int Height);

	HRESULT CreateFromSDLSurface(SDL_Surface* surface);

	bool IsTransparentAt(int X, int Y);
	bool IsTransparentAtLite(int X, int Y);

	HRESULT StartPixelOp();
	HRESULT EndPixelOp();


	HRESULT DisplayTransZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, DWORD Alpha=0xFFFFFFFF, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	HRESULT DisplayTrans(int X, int Y, RECT rect, DWORD Alpha=0xFFFFFFFF, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	HRESULT DisplayTransOffset(int X, int Y, RECT rect, DWORD Alpha=0xFFFFFFFF, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false, int offsetX = 0, int offsetY = 0);
	HRESULT Display(int X, int Y, RECT rect, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	HRESULT DisplayZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, DWORD Alpha=0xFFFFFFFF, bool Transparent=false, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	HRESULT DisplayTransform(int X, int Y, int HotX, int HotY, RECT Rect, float ZoomX, float ZoomY, DWORD Alpha, float Rotate, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);

	static unsigned DLL_CALLCONV ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle);
	static int DLL_CALLCONV SeekProc(fi_handle handle, long offset, int origin);
	static long DLL_CALLCONV TellProc(fi_handle handle);

private:
	SDL_Texture* m_Texture;

	HRESULT DrawSprite(int X, int Y, RECT* Rect, float ZoomX, float ZoomY, DWORD Alpha, bool AlphaDisable, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY, int offsetX = 0, int offsetY = 0);
	void GenAlphaMask(SDL_Surface* surface);
	Uint32 GetPixel(SDL_Surface *surface, int x, int y);

	void* m_LockPixels;
	int m_LockPitch;
	BYTE* m_AlphaMask;
};


#endif // __WmeBSurfaceSDL_H__
