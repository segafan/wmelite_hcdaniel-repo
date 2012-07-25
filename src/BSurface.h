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

#ifndef __WmeBSurface_H__
#define __WmeBSurface_H__


class CBSurface:public CBBase
{
public:
	virtual HRESULT Invalidate();
	virtual HRESULT PrepareToDraw();
	bool m_CKDefault;
	BYTE m_CKRed;
	BYTE m_CKGreen;
	BYTE m_CKBlue;

	DWORD m_LastUsedTime;
	bool m_Valid;
	int m_LifeTime;
	bool m_KeepLoaded;

	bool m_PixelOpReady;
	CBSurface(CBGame* inGame);	
	virtual ~CBSurface();	

	virtual HRESULT DisplayHalfTrans(int X, int Y, RECT rect);
	virtual bool IsTransparentAt(int X, int Y);
	virtual HRESULT DisplayTransZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, DWORD Alpha=0xFFFFFFFF, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	virtual HRESULT DisplayTrans(int X, int Y, RECT rect, DWORD Alpha=0xFFFFFFFF, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	virtual HRESULT DisplayTransOffset(int X, int Y, RECT rect, DWORD Alpha=0xFFFFFFFF, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false, int offsetX = 0, int offsetY = 0);
	virtual HRESULT Display(int X, int Y, RECT rect, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	virtual HRESULT DisplayZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, DWORD Alpha=0xFFFFFFFF, bool Transparent=false, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	virtual HRESULT DisplayTransform(int X, int Y, int HotX, int HotY, RECT Rect, float ZoomX, float ZoomY, DWORD Alpha, float Rotate, TSpriteBlendMode BlendMode=BLEND_NORMAL, bool MirrorX=false, bool MirrorY=false);
	virtual HRESULT Restore();
	virtual HRESULT Create(char* Filename, bool default_ck, BYTE ck_red, BYTE ck_green, BYTE ck_blue, int LifeTime=-1, bool KeepLoaded=false);
	virtual HRESULT Create(int Width, int Height);
	virtual HRESULT PutPixel(int X, int Y, BYTE R, BYTE G, BYTE B, int A=-1);
	virtual HRESULT GetPixel(int X, int Y, BYTE* R, BYTE* G, BYTE* B, BYTE* A=NULL);
	virtual bool ComparePixel(int X, int Y, BYTE R, BYTE G, BYTE B, int A=-1);
	virtual HRESULT StartPixelOp();
	virtual HRESULT EndPixelOp();
	virtual bool IsTransparentAtLite(int X, int Y);
	void SetFilename(char* Filename);
	void SetSize(int Width, int Height);
	
	int m_ReferenceCount;
	char* m_Filename;

	int GetWidth(){ return m_Width;	}
	int GetHeight(){ return m_Height; }
	//void SetWidth(int Width){ m_Width = Width;	}
	//void SetHeight(int Height){ m_Height = Height; }
protected:
	int m_Height;
	int m_Width;

};

#endif
