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
#include "SdlUtil.h"


//////////////////////////////////////////////////////////////////////////
SDL_Texture* SdlUtil::CreateTextureFromSurface(SDL_Renderer* renderer, SDL_Surface* surface)
{
	// this is a copy of SDL_CreateTextureFromSurface, but setting the texture streaming access

	const SDL_PixelFormat *fmt;
	SDL_bool needAlpha;
	Uint32 i;
	Uint32 format;
	SDL_Texture *texture;

	if (!surface) {
		SDL_SetError("SDL_CreateTextureFromSurface() passed NULL surface");
		return NULL;
	}

	/* See what the best texture format is */
	fmt = surface->format;

	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);

	if (fmt->Amask || SDL_GetColorKey(surface, NULL) == 0) {
		needAlpha = SDL_TRUE;
	} else {
		needAlpha = SDL_FALSE;
	}
	format = info.texture_formats[0];
	for (i = 0; i < info.num_texture_formats; ++i) {
		if (!SDL_ISPIXELFORMAT_FOURCC(info.texture_formats[i]) &&
			SDL_ISPIXELFORMAT_ALPHA(info.texture_formats[i]) == needAlpha) {
				format = info.texture_formats[i];
				break;
		}
	}


	texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_STREAMING,
		surface->w, surface->h);
	if (!texture) {
		return NULL;
	}

	if (format == surface->format->format) {
		if (SDL_MUSTLOCK(surface)) {
			SDL_LockSurface(surface);
			SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
			SDL_UnlockSurface(surface);
		} else {
			SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);
		}
	} else {
		SDL_PixelFormat *dst_fmt;
		SDL_Surface *temp = NULL;

		/* Set up a destination surface for the texture update */
		dst_fmt = SDL_AllocFormat(format);
		temp = SDL_ConvertSurface(surface, dst_fmt, 0);
		SDL_FreeFormat(dst_fmt);
		if (temp) {
			SDL_UpdateTexture(texture, NULL, temp->pixels, temp->pitch);
			SDL_FreeSurface(temp);
		} else {
			SDL_DestroyTexture(texture);
			return NULL;
		}
	}

	{
		Uint8 r, g, b, a;
		SDL_BlendMode blendMode;

		SDL_GetSurfaceColorMod(surface, &r, &g, &b);
		SDL_SetTextureColorMod(texture, r, g, b);

		SDL_GetSurfaceAlphaMod(surface, &a);
		SDL_SetTextureAlphaMod(texture, a);

		if (SDL_GetColorKey(surface, NULL) == 0) {
			/* We converted to a texture with alpha format */
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
		} else {
			SDL_GetSurfaceBlendMode(surface, &blendMode);
			SDL_SetTextureBlendMode(texture, blendMode);
		}
	}
	return texture;
}
