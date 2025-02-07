#pragma once

#include <wv/lua.hpp>
#include <vector>

extern "C" {
	int l_wv_log( lua_State* _L );
	int l_wv_logDebug( lua_State* _L );
	
	static const struct luaL_Reg lualib_wvPrint[] = {
		{ "print", l_wv_log },
		{ NULL, NULL }
	};

	static const struct luaL_Reg lualib_wv[] = {
		{ "log", l_wv_log },
		{ "logDebug", l_wv_logDebug },
		{ NULL, NULL }
	};

	extern int luaopen_wv( lua_State* _L );
}