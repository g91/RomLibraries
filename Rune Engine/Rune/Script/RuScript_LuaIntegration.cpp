#include "../Script/RuScript_LuaIntegration.h"

#include "../Fusion/RuFusion_AudioLibrary.h"

#pragma managed(push, off)

#include <luabind/luabind.hpp>

#include "../Fusion/RuFusion_AudioLibrary.h"

using namespace luabind;

BOOL RuFusion_AudioLibrary_Register(lua_State *luaState)
{
	open(luaState);

	module(luaState)
	[
		class_<CRuFusion_AudioLibrary>("CRuFusion_AudioLibrary")
			.def("AssociateAudio", &CRuFusion_AudioLibrary::AssociateAudio),
		def("RuFusion_GetAudioLibrary", &RuFusion_GetAudioLibrary)
	];

	return TRUE;
}











/*

class testclass
{
public:
	testclass(): m_test(0) { MessageBox(NULL, "testclass constructor", "", MB_OK); }
	void print_string() { MessageBox(NULL, "LUA", "", MB_OK); }
	void bar() { MessageBox(NULL, "LUA", "", MB_OK); }
private:
	int m_test;
};

float foo()
{
	return 0.5f;
}

void luaTest()
{
	lua_State* L = lua_open();
	lua_baselibopen(L);
	lua_mathlibopen(L);

	open(L);

	module(L)
	[
		class_<testclass>("testclass")
			.def(constructor<>())
			.def("print_string", &testclass::print_string)
	];

	char *woot = "a = testclass() \n\r a:print_string()\n\r";

	if(lua_dostring(L, woot))
	{
		MessageBox(NULL, "LUAAAAAAAAAAAAA", "", MB_OK);
	}


	lua_close(L);
}
*/
#pragma managed(pop)
