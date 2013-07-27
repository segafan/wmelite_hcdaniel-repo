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

#ifndef __WmeBParser_H__
#define __WmeBParser_H__


#define TOKEN_DEF_START 		\
  enum					\
  { 				\
	TOKEN_NONE = 0,
#define TOKEN_DEF(name) 		\
	TOKEN_ ## name,
#define TOKEN_DEF_END			\
	TOKEN_TOTAL_COUNT			\
  };
#define TOKEN_TABLE_START(name) 	\
  static CBParser::TokenDesc name [] =		\
  {
#define TOKEN_TABLE(name)		\
	{ TOKEN_ ## name, #name },
#define TOKEN_TABLE_END 		\
	{ 0, 0 }				\
  };

#define PARSERR_GENERIC 		-3
#define PARSERR_EOF				-2
#define PARSERR_TOKENNOTFOUND	-1



class CBParser : public CBBase
{
public:
	struct TokenDesc
	{
		long	id;
		const char	*token;
	};

public:
	int ScanStr (const char* in, const char* format, ...);
	char* GetLastOffender();
	void SkipToken(char** buf, char* tok, char* msg = NULL);
	int GetTokenInt(char** buf);
	float GetTokenFloat(char** buf);
	char* GetToken(char** buf);
	char* GetAssignmentText (char **buf);
	char* GetSubText (char **buf, char open, char close);
	void SkipCharacters (char **buf, const char *toSkip);
	long GetCommand (char **buf, TokenDesc * tokens, char **params);
	long GetObject (char **buf, TokenDesc * tokens, char **name, char **data);
	int m_ParserLine;
	char m_LastOffender[255];
	CBParser(CBGame* inGame=NULL);
	virtual ~CBParser();
	char* m_WhiteSpace;
};

#endif
