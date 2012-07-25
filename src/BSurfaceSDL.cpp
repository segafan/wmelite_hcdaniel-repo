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
#include "BSurfaceSDL.h"
#include "BRenderSDL.h"
#include "SdlUtil.h"


//////////////////////////////////////////////////////////////////////////
CBSurfaceSDL::CBSurfaceSDL(CBGame* inGame) : CBSurface(inGame)
{
	m_Texture = NULL;
	m_AlphaMask = NULL;

	m_LockPixels = NULL;
	m_LockPitch = 0;
}

//////////////////////////////////////////////////////////////////////////
CBSurfaceSDL::~CBSurfaceSDL()
{
	if (m_Texture) SDL_DestroyTexture(m_Texture);
	SAFE_DELETE_ARRAY(m_AlphaMask);

	Game->AddMem(-m_Width * m_Height * 4);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::Create(char* Filename, bool default_ck, BYTE ck_red, BYTE ck_green, BYTE ck_blue, int LifeTime, bool KeepLoaded)
{
	CBRenderSDL* renderer = static_cast<CBRenderSDL*>(Game->m_Renderer);


	FreeImageIO io;
	io.read_proc = CBSurfaceSDL::ReadProc;
	io.write_proc = NULL;
	io.seek_proc = CBSurfaceSDL::SeekProc;
	io.tell_proc = CBSurfaceSDL::TellProc;


	CBFile* file = Game->m_FileManager->OpenFile(Filename);
	if (!file) return E_FAIL;


	if(default_ck)
	{
		ck_red   = 255;
		ck_green = 0;
		ck_blue  = 255;
	}

	
	FREE_IMAGE_FORMAT imgFormat = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)file, 0);
	FIBITMAP* img = FreeImage_LoadFromHandle(imgFormat, &io, (fi_handle)file, 0);
	Game->m_FileManager->CloseFile(file);
	
	if (!img) return E_FAIL;

	m_Width = FreeImage_GetWidth(img);
	m_Height = FreeImage_GetHeight(img);


	bool isSaveGameGrayscale = CBPlatform::strnicmp(Filename, "savegame:", 9)==0 && (Filename[strlen(Filename)-1]=='g' || Filename[strlen(Filename)-1]=='G');
	if (isSaveGameGrayscale)
	{
		FIBITMAP* newImg = FreeImage_ConvertToGreyscale(img);
		if (newImg)
		{
			FreeImage_Unload(img);
			img = newImg;
		}
	}

	// convert 32-bit BMPs to 24-bit or they appear totally transparent (does any app actually write alpha in BMP properly?)
	if (FreeImage_GetBPP(img) != 32 || (imgFormat == FIF_BMP && FreeImage_GetBPP(img) != 24))
	{
		FIBITMAP* newImg = FreeImage_ConvertTo24Bits(img);
		if (newImg)
		{
			FreeImage_Unload(img);
			img = newImg;
		}
		else
		{
			FreeImage_Unload(img);
			return -1;
		}
	}

	FreeImage_FlipVertical(img);


	void* inPtr = FreeImage_GetBits(img); 
	SDL_Surface* surf = SDL_CreateRGBSurfaceFrom(inPtr, m_Width, m_Height, FreeImage_GetBPP(img), FreeImage_GetPitch(img), FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FI_RGBA_ALPHA_MASK); 

	// no alpha, set color key
	if (FreeImage_GetBPP(img) != 32)
		SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, ck_red, ck_green, ck_blue));

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	//m_Texture = SdlUtil::CreateTextureFromSurface(renderer->GetSdlRenderer(), surf);
	m_Texture = SDL_CreateTextureFromSurface(renderer->GetSdlRenderer(), surf);
	if (!m_Texture)
	{
		SDL_FreeSurface(surf);
		FreeImage_Unload(img);
		return E_FAIL;
	}

	GenAlphaMask(surf);

	SDL_FreeSurface(surf);
	FreeImage_Unload(img);


	m_CKDefault = default_ck;
	m_CKRed = ck_red;
	m_CKGreen = ck_green;
	m_CKBlue = ck_blue;


	if (!m_Filename || CBPlatform::stricmp(m_Filename, Filename) !=0)
	{
		SetFilename(Filename);
	}

	if(m_LifeTime==0 || LifeTime==-1 || LifeTime>m_LifeTime)
		m_LifeTime = LifeTime;

	m_KeepLoaded = KeepLoaded;
	if(m_KeepLoaded) m_LifeTime = -1;
	
	m_Valid = true;

	Game->AddMem(m_Width * m_Height * 4);
	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
void CBSurfaceSDL::GenAlphaMask(SDL_Surface* surface)
{
	SAFE_DELETE_ARRAY(m_AlphaMask);
	if (!surface) return;

	SDL_LockSurface(surface);

	bool hasColorKey;
	Uint32 colorKey;
	Uint8 ckRed, ckGreen, ckBlue;
	if (SDL_GetColorKey(surface, &colorKey) == 0)
	{
		hasColorKey = true;
		SDL_GetRGB(colorKey, surface->format, &ckRed, &ckGreen, &ckBlue);
	}
	else hasColorKey = false;

	m_AlphaMask = new BYTE[surface->w * surface->h];

	bool hasTransparency = false;
	for (int y = 0; y < surface->h; y++)
	{
		for (int x = 0; x < surface->w; x++)
		{
			Uint32 pixel = GetPixel(surface, x, y);

			Uint8 r, g, b, a;
			SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

			if (hasColorKey && r == ckRed && g == ckGreen && b == ckBlue)
				a = 0;

			m_AlphaMask[y * surface->w + x] = a;
			if (a < 255) hasTransparency = true;
		}
	}

	SDL_UnlockSurface(surface);

	if (!hasTransparency) SAFE_DELETE_ARRAY(m_AlphaMask);
}

//////////////////////////////////////////////////////////////////////////
Uint32 CBSurfaceSDL::GetPixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch(bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16 *)p;
		break;

	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32 *)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::Create(int Width, int Height)
{
	CBRenderSDL* renderer = static_cast<CBRenderSDL*>(Game->m_Renderer);
	m_Texture = SDL_CreateTexture(renderer->GetSdlRenderer(), SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, Width, Height);

	m_Width = Width;
	m_Height = Height;

	Game->AddMem(m_Width * m_Height * 4);

	m_Valid = true;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::CreateFromSDLSurface(SDL_Surface* surface)
{
	CBRenderSDL* renderer = static_cast<CBRenderSDL*>(Game->m_Renderer);
	m_Texture = SDL_CreateTextureFromSurface(renderer->GetSdlRenderer(), surface);

	m_Width = surface->w;
	m_Height = surface->h;

	Game->AddMem(m_Width * m_Height * 4);

	m_Valid = true;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
bool CBSurfaceSDL::IsTransparentAt(int X, int Y)
{
	int access;
	int width, height;
	SDL_QueryTexture(m_Texture, NULL, &access, &width, &height);
	//if (access != SDL_TEXTUREACCESS_STREAMING) return false;
	if (X < 0 || X >= width || Y < 0 || Y >= height) return true;


	StartPixelOp();
	bool ret = IsTransparentAtLite(X, Y);
	EndPixelOp();

	return ret;
}

//////////////////////////////////////////////////////////////////////////
bool CBSurfaceSDL::IsTransparentAtLite(int X, int Y)
{
	//if (!m_LockPixels) return false;

	Uint32 format;
	int access;
	int width, height;
	SDL_QueryTexture(m_Texture, &format, &access, &width, &height);
	//if (access != SDL_TEXTUREACCESS_STREAMING) return false;
	if (X < 0 || X >= width || Y < 0 || Y >= height) return true;

	if (!m_AlphaMask) return false;
	else return m_AlphaMask[Y * width + X] <= 128;

	/*
	Uint32* dst = (Uint32*)((Uint8*)m_LockPixels + Y * m_LockPitch);
	Uint32 pixel = dst[X];

	SDL_PixelFormat* pixelFormat = SDL_AllocFormat(format);
	Uint8 r, g, b, a;
	SDL_GetRGBA(pixel, pixelFormat, &r, &g, &b, &a);
	SDL_FreeFormat(pixelFormat);	

	return a <= 128;
	*/
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::StartPixelOp()
{
	//SDL_LockTexture(m_Texture, NULL, &m_LockPixels, &m_LockPitch);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::EndPixelOp()
{
	//SDL_UnlockTexture(m_Texture);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::Display(int X, int Y, RECT rect, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY)
{
	return DrawSprite(X, Y, &rect, 100, 100, 0xFFFFFFFF, true, BlendMode, MirrorX, MirrorY);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayTrans(int X, int Y, RECT rect, DWORD Alpha, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY)
{
	return DrawSprite(X, Y, &rect, 100, 100, Alpha, false, BlendMode, MirrorX, MirrorY);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayTransOffset(int X, int Y, RECT rect, DWORD Alpha, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY, int offsetX, int offsetY)
{
	return DrawSprite(X, Y, &rect, 100, 100, Alpha, false, BlendMode, MirrorX, MirrorY, offsetX, offsetY);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayTransZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, DWORD Alpha, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY)
{
	return DrawSprite(X, Y, &rect, ZoomX, ZoomY, Alpha, false, BlendMode, MirrorX, MirrorY);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayZoom(int X, int Y, RECT rect, float ZoomX, float ZoomY, DWORD Alpha, bool Transparent, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY)
{
	return DrawSprite(X, Y, &rect, ZoomX, ZoomY, Alpha, !Transparent, BlendMode, MirrorX, MirrorY);
}


//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DisplayTransform(int X, int Y, int HotX, int HotY, RECT Rect, float ZoomX, float ZoomY, DWORD Alpha, float Rotate, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY)
{
	return DrawSprite(X, Y, &Rect, ZoomX, ZoomY, Alpha, false, BlendMode, MirrorX, MirrorY);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSurfaceSDL::DrawSprite(int X, int Y, RECT* Rect, float ZoomX, float ZoomY, DWORD Alpha, bool AlphaDisable, TSpriteBlendMode BlendMode, bool MirrorX, bool MirrorY, int offsetX, int offsetY)
{
	CBRenderSDL* renderer = static_cast<CBRenderSDL*>(Game->m_Renderer);

	if (renderer->m_ForceAlphaColor != 0) Alpha = renderer->m_ForceAlphaColor;

	BYTE r = D3DCOLGetR(Alpha);
	BYTE g = D3DCOLGetG(Alpha);
	BYTE b = D3DCOLGetB(Alpha);
	BYTE a = D3DCOLGetA(Alpha);

	SDL_SetTextureColorMod(m_Texture, r, g, b);
	SDL_SetTextureAlphaMod(m_Texture, a);

	if (AlphaDisable)
		SDL_SetTextureBlendMode(m_Texture, SDL_BLENDMODE_NONE);
	else
		SDL_SetTextureBlendMode(m_Texture, SDL_BLENDMODE_BLEND);

	SDL_Rect srcRect;
	srcRect.x = Rect->left;
	srcRect.y = Rect->top;
	srcRect.w = Rect->right - Rect->left;
	srcRect.h = Rect->bottom - Rect->top;

	SDL_Rect position;
	position.x = X;
	position.y = Y;
	position.w = (float)srcRect.w * ZoomX / 100.f;
	position.h = (float)srcRect.h * ZoomX / 100.f;

	renderer->ModTargetRect(&position);

	position.x += offsetX;
	position.y += offsetY;

	SDL_RenderCopy(renderer->GetSdlRenderer(), m_Texture, &srcRect, &position);


	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
unsigned CBSurfaceSDL::ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
	CBFile* file = static_cast<CBFile*>(handle);
	file->Read(buffer, size * count);
	return count;
}

//////////////////////////////////////////////////////////////////////////
int CBSurfaceSDL::SeekProc(fi_handle handle, long offset, int origin)
{
	CBFile* file = static_cast<CBFile*>(handle);
	file->Seek(offset, (TSeek)origin);
	return 0;
}

//////////////////////////////////////////////////////////////////////////
long CBSurfaceSDL::TellProc(fi_handle handle)
{
	CBFile* file = static_cast<CBFile*>(handle);
	return file->GetPos();
}
