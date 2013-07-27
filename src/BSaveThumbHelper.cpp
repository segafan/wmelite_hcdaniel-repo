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
#include "BSaveThumbHelper.h"


//////////////////////////////////////////////////////////////////////////
CBSaveThumbHelper::CBSaveThumbHelper(CBGame* inGame):CBBase(inGame)
{
	m_Thumbnail = NULL;
}

//////////////////////////////////////////////////////////////////////////
CBSaveThumbHelper::~CBSaveThumbHelper(void)
{
	SAFE_DELETE(m_Thumbnail);
}

//////////////////////////////////////////////////////////////////////////
HRESULT CBSaveThumbHelper::StoreThumbnail(bool DoFlip)
{
	SAFE_DELETE(m_Thumbnail);

	if (Game->m_ThumbnailWidth > 0 && Game->m_ThumbnailHeight > 0)
	{
		if(DoFlip)
		{
			// when using opengl on windows it seems to be necessary to do this twice
			// works normally for direct3d
			Game->DisplayContent(false);
			Game->m_Renderer->Flip();
			
			Game->DisplayContent(false);
			Game->m_Renderer->Flip();
		}

		CBImage* Screenshot = Game->m_Renderer->TakeScreenshot();
		if(!Screenshot) return E_FAIL;

		// normal thumbnail
		if(Game->m_ThumbnailWidth > 0 && Game->m_ThumbnailHeight > 0)
		{
			m_Thumbnail = new CBImage(Game);
			m_Thumbnail->CopyFrom(Screenshot, Game->m_ThumbnailWidth, Game->m_ThumbnailHeight);
		}


		SAFE_DELETE(Screenshot);
	}
	return S_OK;
}