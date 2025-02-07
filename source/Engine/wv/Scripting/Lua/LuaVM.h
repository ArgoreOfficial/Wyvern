#pragma once

#include <string>
#include <vector>

struct lua_State;

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
	LuaStackGuard( lua_State* _L );

	bool check();

private:
	int m_stackBegin = 0;
	lua_State* L;
};

class LuaVM
{
public:
	LuaVM();
	~LuaVM();

	lua_State* getState() { 
		return m_luaState; 
	}

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
		LuaStackGuard stackGuard{ m_luaState };
		
		if( _loadFunc( _funcName ) != LuaResult::kSuccess )
			return kError;
		pushArgsToStack( _args... );
		_execFunc();
		
		if( !stackGuard.check() )
			return kError_StackMismatch;

		return kSuccess;
	}

private:

	LuaResult _loadFunc( const std::string& _funcName );
	LuaResult _execFunc();

	lua_State* m_luaState = nullptr;


};

}
