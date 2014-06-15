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

#ifndef __WmeBRenderSDL_H__
#define __WmeBRenderSDL_H__

#include "BRenderer.h"
#include "SDL.h"

class CBRenderSDL : public CBRenderer
{
public:
	CBRenderSDL(CBGame* inGame);
	~CBRenderSDL();

	const char* GetName();

	HRESULT InitRenderer(int width, int height, bool windowed, float upScalingRatioStepping, float downScalingRatioStepping, bool pixelPerfectRendering, const AnsiString& renderingHint, bool vsync);
	HRESULT Flip();
	HRESULT EraseBackground();
	HRESULT Fill(BYTE r, BYTE g, BYTE b, RECT* rect);

	HRESULT Fade(WORD Alpha);
	HRESULT FadeToColor(DWORD Color, RECT* rect = NULL);

	HRESULT SwitchFullscreen();

	HRESULT DrawLine(int X1, int Y1, int X2, int Y2, DWORD Color);

	CBImage* TakeScreenshot();

	SDL_Renderer* GetSdlRenderer() const { return m_Renderer; }
	SDL_Window* GetSdlWindow() const { return m_Win; }

	HRESULT SetViewport(int left, int top, int right, int bottom);

	HRESULT SendRenderingHintSceneComplete();

	void ModTargetRect(SDL_Rect* rect);
	void ModOrigin(SDL_Point* origin);
	void PointFromScreen(POINT* point);
	void PointToScreen(POINT* point);

	void DumpData(char* Filename);

	float GetScaleRatioX() const { return m_RatioX; }
	float GetScaleRatioY() const { return m_RatioY; }

private:
	SDL_Renderer* m_Renderer;
	SDL_Texture* m_Texture;
	SDL_Window* m_Win;
	AnsiString m_Name;

	int m_BorderLeft;
	int m_BorderTop;
	int m_BorderRight;
	int m_BorderBottom;

	float m_RatioX;
	float m_RatioY;

	// whether pixelperfect is active at all
	bool m_PixelPerfect;
	// whether the current frame is still rendered offscreen, or directly (for fonts)
	bool m_RenderOffscreen;
	SDL_Rect m_PixelPerfectTargetRect;

	DWORD m_LastRenderTime;
	DWORD m_FrameRateLimit;
	DWORD m_FrameSleepTime;
	DWORD m_SecondTickTime;
	DWORD m_FrameCounter;

	float GetAlignedUpscalingRatio(float ratio, float stepping);
	float GetAlignedDownscalingRatio(float ratio, float stepping);

};


#endif // __WmeBRenderSDL_H__
