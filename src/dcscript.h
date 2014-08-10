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

#ifndef _DCSCRIPT_H_
#define _DCSCRIPT_H_


#define SCRIPT_MAGIC   0xDEC0ADDE
#define SCRIPT_VERSION 0x0102

// value types
typedef enum{
	VAL_NULL,
	VAL_STRING,
	VAL_INT,
	VAL_BOOL,
	VAL_FLOAT,
	VAL_OBJECT,
	VAL_NATIVE,
	VAL_VARIABLE_REF
} TValType;


// script states
typedef enum{
	SCRIPT_RUNNING, SCRIPT_WAITING, SCRIPT_SLEEPING, SCRIPT_FINISHED, SCRIPT_PERSISTENT, SCRIPT_ERROR, SCRIPT_PAUSED, SCRIPT_WAITING_SCRIPT, SCRIPT_THREAD_FINISHED
} TScriptState;

// opcodes
typedef enum {
	II_DEF_VAR = 0,
	II_DEF_GLOB_VAR,
	II_RET,
	II_RET_EVENT,
	II_CALL,
	II_CALL_BY_EXP,
	II_EXTERNAL_CALL,
	II_SCOPE,
	II_CORRECT_STACK,
	II_CREATE_OBJECT,
	II_POP_EMPTY,
	II_PUSH_VAR,
	II_PUSH_VAR_REF,
	II_POP_VAR,
	II_PUSH_VAR_THIS, // push current this on stack
	II_PUSH_INT,
	II_PUSH_BOOL,
	II_PUSH_FLOAT,
	II_PUSH_STRING,
	II_PUSH_NULL,
	II_PUSH_THIS_FROM_STACK,
	II_PUSH_THIS,
	II_POP_THIS,
	II_PUSH_BY_EXP,
	II_POP_BY_EXP,
	II_JMP,
	II_JMP_FALSE,
	II_ADD,
	II_SUB,
	II_MUL,
	II_DIV,
	II_MODULO,
	II_NOT,
	II_AND,
	II_OR,
	II_CMP_EQ,
	II_CMP_NE,
	II_CMP_L,
	II_CMP_G,
	II_CMP_LE,
	II_CMP_GE,
	II_CMP_STRICT_EQ,
	II_CMP_STRICT_NE,
	II_DBG_LINE,
	II_POP_REG1,
	II_PUSH_REG1,
	II_DEF_CONST_VAR
} TInstruction;

// external data types
typedef enum{
	TYPE_VOID = 0,
	TYPE_BOOL,
	TYPE_LONG,
	TYPE_BYTE,
	TYPE_STRING,
	TYPE_FLOAT,
	TYPE_DOUBLE,
	TYPE_MEMBUFFER
} TExternalType;


// call types
typedef enum{
	CALL_STDCALL = 0,
	CALL_CDECL,
	CALL_THISCALL
} TCallType;

// element types
typedef enum{
	ELEMENT_STRING = 0
} TElementType;



// compiler interface
typedef BYTE* (WINAPI DLL_LOAD_FILE)  (void* Data, char* Filename, DWORD* Size);
typedef void  (WINAPI DLL_CLOSE_FILE) (void* Data, BYTE* Buffer);
typedef void  (WINAPI DLL_ADD_ERROR)  (void* Data, int Line, char* Text);
typedef void  (WINAPI DLL_PARSE_ELEMENT)  (void* Data, int Line, int Type, void* ElementData);

typedef struct{
	DLL_LOAD_FILE* Dll_LoadFile;
	DLL_CLOSE_FILE* Dll_CloseFile;
	DLL_ADD_ERROR* Dll_AddError;
	DLL_PARSE_ELEMENT* Dll_ParseElement;
} CALLBACKS;

#endif
