#ifndef _RUSCRIPT_LUAINTEGRATION_H_
#define _RUSCRIPT_LUAINTEGRATION_H_

#pragma managed(push, off)

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

#include "../Core/Type/RuType_Object.h"

extern BOOL RuFusion_AudioLibrary_Register(lua_State *luaState);

extern void luaTest();

#pragma managed(pop)

#endif
