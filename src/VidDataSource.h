/*
This file is part of WME Lite.
http://dead-code.org/redir.php?target=wmelite

Copyright (c) 2013 Jan Nedoma

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

#ifndef __WmeVidDataSource_H__
#define __WmeVidDataSource_H__

#include <theoraplayer/TheoraDataSource.h>


class CVidDataSource : public TheoraDataSource
{
public:	
	CVidDataSource(CBGame* inGame, CBFile* file, const char* fileName);
	virtual ~CVidDataSource();

	int read(void* output,int nBytes);
	std::string repr();
	void seek(unsigned long byte_index);
	unsigned long size();
	unsigned long tell();

private:
	CBGame* m_Game;
	CBFile* m_File;
	AnsiString m_Filename;
};

#endif // __WmeVidDataSource_H__
