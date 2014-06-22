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

#include "dcgf.h"
#include "BImage.h"


//////////////////////////////////////////////////////////////////////
CBImage::CBImage(CBGame* inGame, FIBITMAP* bitmap):CBBase(inGame)
{
	m_Bitmap = bitmap;
}


//////////////////////////////////////////////////////////////////////
CBImage::~CBImage()
{
	if (m_Bitmap) FreeImage_Unload(m_Bitmap);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBImage::SaveBMPFile(char* Filename)
{
	if (!m_Bitmap) return E_FAIL;

	if (FreeImage_Save(FIF_BMP, m_Bitmap, Filename)) return S_OK;
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBImage::Resize(int NewWidth, int NewHeight)
{
	if (!m_Bitmap) return E_FAIL;

	if (NewWidth == 0) NewWidth = FreeImage_GetWidth(m_Bitmap);
	if (NewHeight == 0) NewHeight = FreeImage_GetHeight(m_Bitmap);


	FIBITMAP* newImg = FreeImage_Rescale(m_Bitmap, NewWidth, NewHeight, FILTER_BILINEAR);
	if (newImg)
	{
		FreeImage_Unload(m_Bitmap);
		m_Bitmap = newImg;
		return S_OK;
	}
	else return E_FAIL;
}


//////////////////////////////////////////////////////////////////////////
BYTE* CBImage::CreateBMPBuffer(DWORD* BufferSize)
{
	if (!m_Bitmap) return NULL;

	FIMEMORY* fiMem = FreeImage_OpenMemory();
	FreeImage_SaveToMemory(FIF_PNG, m_Bitmap, fiMem);
	DWORD size;
	BYTE* data;
	FreeImage_AcquireMemory(fiMem, &data, &size);

	
	BYTE* Buffer = new BYTE[size];
	memcpy(Buffer, data, size);

	FreeImage_CloseMemory(fiMem);

	if (BufferSize) *BufferSize = size;

	return Buffer;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBImage::CopyFrom(CBImage* OrigImage, int NewWidth, int NewHeight)
{
	if (m_Bitmap) FreeImage_Unload(m_Bitmap);

	if (NewWidth == 0) NewWidth = FreeImage_GetWidth(OrigImage->GetBitmap());
	if (NewHeight == 0) NewHeight = FreeImage_GetHeight(OrigImage->GetBitmap());

	m_Bitmap = FreeImage_Rescale(OrigImage->GetBitmap(), NewWidth, NewHeight, FILTER_BILINEAR);

	return S_OK;
}
