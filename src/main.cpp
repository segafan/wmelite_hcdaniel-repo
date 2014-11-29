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

#ifdef _MSC_VER
#	ifdef _DEBUG
#		define _CRTDBG_MAP_ALLOC
#		include <stdlib.h>
#		include <crtdbg.h>
#	endif
#endif

#include "SDL.h"
#include "PlatformSDL.h"
#include "dcgf.h"

#ifdef __WIN32__
#	include "../visualc/resource.h"
#endif

//////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
#ifdef _MSC_VER
#	ifdef _DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
	//_CrtSetBreakAlloc(53403);
#	endif
#endif

#ifndef __WIN32__
	FreeImage_Initialise();
#endif
	int ret = 1;

	CAdGame* Game = new CAdGame;
	Game->m_SmartCache = true;

	ret = CBPlatform::Initialize(Game, argc, argv);
	if (ret == 0)
	{
		ret = CBPlatform::MessageLoop();
	}


#ifndef __WIN32__
	FreeImage_DeInitialise();
#endif



	return ret;
}
