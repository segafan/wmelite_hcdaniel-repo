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
#include "Matrix4.h"
#include "Vector2.h"
#include <math.h>


//////////////////////////////////////////////////////////////////////////
Matrix4::Matrix4()
{
	_11 = _12 = _13 = _14 = 0.0f;
	_21 = _22 = _23 = _24 = 0.0f;
	_31 = _32 = _33 = _34 = 0.0f;
	_41 = _42 = _43 = _44 = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
Matrix4::~Matrix4()
{
}


//////////////////////////////////////////////////////////////////////////
void Matrix4::Identity()
{
	_12 = _13 = _14 = 0.0f;
	_21 = _23 = _24 = 0.0f;
	_31 = _32 = _34 = 0.0f;
	_41 = _42 = _43 = 0.0f;

	_11 = _22 = _33 = _44 = 1.0f;
}

//////////////////////////////////////////////////////////////////////////
void Matrix4::RotationZ(float angle)
{
	Identity();

	m[0][0] = cos(angle);
	m[1][1] = cos(angle);
	m[0][1] = sin(angle);
	m[1][0] = -sin(angle);
}

//////////////////////////////////////////////////////////////////////////
void Matrix4::TransformVector2(Vector2& vec)
{
	float norm;
	
	norm = m[0][3] * vec.x + m[1][3] * vec.y + m[3][3];

	float x = (m[0][0] * vec.x + m[1][0] * vec.y + m[3][0]) / norm;
	float y = (m[0][1] * vec.x + m[1][1] * vec.y + m[3][1]) / norm;

	vec.x = x;
	vec.y = y;
}
