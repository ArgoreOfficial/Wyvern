#pragma once

#include <string>
#include <vector>

#include <wv/lua.hpp>

namespace wv {

enum LuaResult
{
	kSuccess = 0,
	kError,
	kError_Load,
	kError_Run,
	kError_StackMismatch
};

struct LuaStackGuard
{
	LuaStackGuard( lua_State* _luaState );

	bool check();

private:
	int m_stackBegin = 0;
	lua_State* m_luaState;
};

class LuaVM
{
public:
	LuaVM();
	~LuaVM();

	lua_State* getState() { 
		return m_luaState; 
	}

	void addLibrary( const char* _name, lua_CFunction _loadfunc );

	template<typename _Ty>
	int pushToStack( const _Ty& _v ) {
		static_assert( false, "Invalid type" );
		return 0;
	}

	template<typename... _Args>
	void pushArgsToStack( _Args... _args ) {
		std::vector err = { pushToStack( _args )... };
	}

	LuaResult runScript( const std::string& _script );

	template<typename... _Args>
	LuaResult runGlobalFunction( const std::string& _funcName, _Args... _args ) {
		
		if( _loadFunc( _funcName ) != LuaResult::kSuccess )
			return kError;
		pushArgsToStack( _args... );
		
		return _execFunc( sizeof...( _Args ) );
	}

	double popReturnNumber();

private:

	void _loadLibs( int _load, int _preload );

	LuaResult _loadFunc( const std::string& _funcName );
	LuaResult _execFunc( int _numArgs );

	int _pcall( int _nargs, int _nret, int _hpos );

	lua_State* m_luaState = nullptr;

	std::vector<luaL_Reg> m_libs = {};

};

}
