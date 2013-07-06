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

#ifndef __WmeAdTypes_H__
#define __WmeAdTypes_H__


typedef enum
{
	GAME_NORMAL, GAME_WAITING_RESPONSE
} TGameStateEx;


typedef enum
{
	OBJECT_ENTITY, OBJECT_REGION, OBJECT_ACTOR, OBJECT_NONE
} TObjectType;


typedef enum
{
	ENTITY_NORMAL, ENTITY_SOUND
} TEntityType;


typedef enum
{
	STATE_NONE,
	STATE_IDLE,
	STATE_PLAYING_ANIM,
	STATE_READY,
	STATE_FOLLOWING_PATH,
	STATE_SEARCHING_PATH,
	STATE_WAITING_PATH,
	STATE_TURNING_LEFT,
	STATE_TURNING_RIGHT,
	STATE_TURNING,
	STATE_TALKING,
	STATE_DIRECT_CONTROL,
	STATE_PLAYING_ANIM_SET,
} TObjectState;

typedef enum
{
	DIRECT_WALK_NONE, DIRECT_WALK_FW, DIRECT_WALK_BK
} TDirectWalkMode;

typedef enum
{
	DIRECT_TURN_NONE, DIRECT_TURN_CW, DIRECT_TURN_CCW
} TDirectTurnMode;

typedef enum
{
	RESPONSE_TEXT, RESPONSE_ICON
} TResponseStyle;

typedef enum
{
	RESPONSE_ALWAYS, RESPONSE_ONCE, RESPONSE_ONCE_GAME
} TResponseType;


typedef enum
{
	TALK_SKIP_LEFT=0, TALK_SKIP_RIGHT=1, TALK_SKIP_BOTH=2, TALK_SKIP_NONE=3
} TTalkSkipButton;


typedef enum
{
	VIDEO_SKIP_LEFT=0, VIDEO_SKIP_RIGHT=1, VIDEO_SKIP_BOTH=2, VIDEO_SKIP_NONE=3
} TVideoSkipButton;


typedef enum
{
	GEOM_WAYPOINT, GEOM_WALKPLANE, GEOM_BLOCKED, GEOM_GENERIC
} TGeomNodeType;


#endif // __WmeAdTypes_H__
