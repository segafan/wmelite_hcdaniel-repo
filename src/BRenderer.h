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

#ifndef __WmeBRenderer_H__
#define __WmeBRenderer_H__


#include "coll_templ.h"

class CBImage;

class CBRenderer:public CBBase
{
public:
	int m_RealWidth;
	int m_RealHeight;
	int m_DrawOffsetX;
	int m_DrawOffsetY;

	virtual void DumpData(char* Filename){};
	virtual CBImage* TakeScreenshot();
	virtual HRESULT SetViewport(int left, int top, int right, int bottom);
	virtual HRESULT SetViewport(RECT* Rect);
	virtual HRESULT SetScreenViewport();
	virtual HRESULT Fade(WORD Alpha);
	virtual HRESULT FadeToColor(DWORD Color, RECT* rect=NULL);
	virtual HRESULT DrawLine(int X1, int Y1, int X2, int Y2, DWORD Color);
	virtual HRESULT DrawRect(int X1, int Y1, int X2, int Y2, DWORD Color, int Width=1);
	CBRenderer(CBGame* inGame=NULL);
	virtual ~CBRenderer();
	virtual HRESULT SetProjection() { return S_OK; };

	virtual HRESULT WindowedBlt();	
	virtual HRESULT Fill(BYTE r, BYTE g, BYTE b, RECT* rect=NULL);
	virtual void OnWindowChange();
	virtual HRESULT InitRenderer(int width, int height, bool windowed, float upScalingRatioStepping, float downScalingRatioStepping, bool pixelPerfectRendering);
	virtual HRESULT Flip();
	virtual void InitLoop();
	virtual HRESULT SwitchFullscreen();
	virtual HRESULT Setup2D(bool Force=false);
	virtual HRESULT SetupLines();

	virtual const char* GetName(){ return ""; };
	virtual HRESULT DisplayDebugInfo() { return E_FAIL; };
	virtual HRESULT DrawShaderQuad() { return E_FAIL; }

	virtual float GetScaleRatioX() const { return 1.0f; }
	virtual float GetScaleRatioY() const { return 1.0f; }

	HRESULT ClipCursor();
	HRESULT UnclipCursor();

	CBObject* GetObjectAt(int X, int Y);
	void DeleteRectList();

	virtual HRESULT StartSpriteBatch() { return S_OK; };
	virtual HRESULT EndSpriteBatch() { return S_OK; };
	bool PointInViewport(POINT* P);
	DWORD m_ForceAlphaColor;
	HINSTANCE m_Instance;
	HWND m_Window;
	HWND m_ClipperWindow;
	bool m_Active;
	bool m_Ready;
	bool m_Windowed;
	RECT m_WindowRect;
	RECT m_ViewportRect;
	RECT m_ScreenRect;
	RECT m_MonitorRect;
	int m_BPP;	
	int m_Height;
	int m_Width;

	CBArray<CBActiveRect*, CBActiveRect*> m_RectList;
};

#endif
