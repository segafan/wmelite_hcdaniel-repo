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

#ifndef __WmeBFontTT_H__
#define __WmeBFontTT_H__


#include "BFont.h"

#define NUM_CACHED_TEXTS 30

class FontGlyphCache;

class CBFontTT : public CBFont
{
private:
	//////////////////////////////////////////////////////////////////////////
	class CBCachedTTFontText
	{
	public:
		WideString m_Text;
		int m_Width;
		TTextAlign m_Align;
		int m_MaxHeight;
		int m_MaxLength;
		CBSurface* m_Surface;
		int m_Priority;
		int m_TextOffset;
		bool m_Marked;

		CBCachedTTFontText()
		{
			m_Text = L"";
			m_Width = m_MaxHeight = m_MaxLength = -1;
			m_Align = TAL_LEFT;
			m_Surface = NULL;
			m_Priority = -1;
			m_TextOffset = 0;
			m_Marked = false;
		}

		virtual ~CBCachedTTFontText()
		{
			if(m_Surface) delete m_Surface;
		}
	};

public:
	//////////////////////////////////////////////////////////////////////////
	class CBTTFontLayer
	{
	public:
		CBTTFontLayer()
		{
			m_OffsetX = m_OffsetY = 0;
			m_Color = 0x00000000;
		}
		
		HRESULT Persist(CBPersistMgr* PersistMgr)
		{
			PersistMgr->Transfer(TMEMBER(m_OffsetX));
			PersistMgr->Transfer(TMEMBER(m_OffsetY));
			PersistMgr->Transfer(TMEMBER(m_Color));
			return S_OK;
		}
		
		int m_OffsetX;
		int m_OffsetY;
		DWORD m_Color;
	};

	//////////////////////////////////////////////////////////////////////////
	class TextLine
	{
	public:
		TextLine(const WideString& text, int width)
		{
			m_Text = text;
			m_Width = width;
		}

		const WideString& GetText() const { return m_Text; }
		int GetWidth() const { return m_Width; }
	private:
		WideString m_Text;
		int m_Width;
	};
	typedef std::list<TextLine*> TextLineList;


public:
	DECLARE_PERSISTENT(CBFontTT, CBFont);
	CBFontTT(CBGame* inGame);
	virtual ~CBFontTT(void);

	virtual int GetTextWidth(BYTE* text, int MaxLenght=-1);
	virtual int GetTextHeight(BYTE* text, int width);
	virtual void DrawText(BYTE* text, int x, int y, int width, TTextAlign align=TAL_LEFT, int max_height=-1, int MaxLenght=-1);
	virtual int GetLetterHeight();

	HRESULT LoadBuffer(BYTE* Buffer);
	HRESULT LoadFile(char* Filename);

	static unsigned long FTReadSeekProc(FT_Stream stream, unsigned long offset,	unsigned char* buffer, unsigned long count);
	static void FTCloseProc(FT_Stream stream);

	FontGlyphCache* GetGlyphCache() { return m_GlyphCache; }

	float GetLineHeight() const { return m_LineHeight; }

	void AfterLoad();
	void InitLoop();

private:
	HRESULT ParseLayer(CBTTFontLayer* Layer, BYTE* Buffer);

	int WrapText(const WideString& text, int maxWidth, int maxHeight, TextLineList& lines);
	void MeasureText(const WideString& text, int maxWidth, int maxHeight, int& textWidth, int& textHeight);
	float GetKerning(wchar_t leftChar, wchar_t rightChar);
	void PrepareGlyphs(const WideString& text);
	void CacheGlyph(wchar_t ch);

	CBSurface* RenderTextToTexture(const WideString& text, int width, TTextAlign align, int maxHeight, int& textOffset);
	void BlitSurface(SDL_Surface* src, SDL_Surface* target, SDL_Rect* targetRect);


	CBCachedTTFontText* m_CachedTexts[NUM_CACHED_TEXTS];

	HRESULT InitFont();
	FT_Stream m_FTStream;
	FT_Face m_FTFace;

	FontGlyphCache* m_GlyphCache;

	float m_Ascender;
	float m_Descender;
	float m_LineHeight;
	float m_UnderlinePos;
	float m_PointSize;
	float m_VertDpi;
	float m_HorDpi;

	size_t m_MaxCharWidth;
	size_t m_MaxCharHeight;

	bool m_FontAlphaHack;

	int m_FontYOffsetCompensation;

public:
	bool m_IsBold;
	bool m_IsItalic;
	bool m_IsUnderline;
	bool m_IsStriked;
	int m_FontHeight;
	char* m_FontFile;

	CBArray<CBTTFontLayer*, CBTTFontLayer*> m_Layers;
	void ClearCache();
	
};

#endif
