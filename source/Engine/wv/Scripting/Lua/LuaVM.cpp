#include "LuaVM.h"

extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

#include <wv/Debug/Print.h>

template<> int wv::LuaVM::pushToStack( const int&         _v ) { lua_pushinteger( m_luaState, _v ); return 1; }
template<> int wv::LuaVM::pushToStack( const bool&        _v ) { lua_pushboolean( m_luaState, _v ); return 1; }
template<> int wv::LuaVM::pushToStack( const std::string& _v ) { lua_pushlstring( m_luaState, _v.c_str(), _v.size() ); return 1; }
template<> int wv::LuaVM::pushToStack( const double&      _v ) { lua_pushnumber ( m_luaState, _v ); return 1; }

wv::LuaStackGuard::LuaStackGuard( lua_State* _L )
{
    L = _L;
    m_stackBegin = lua_gettop( L );
}

bool wv::LuaStackGuard::check()
{
    return lua_gettop( L ) == m_stackBegin;
}

wv::LuaVM::LuaVM()
{
	m_luaState = luaL_newstate();

	luaL_openlibs( m_luaState ); // dangerous
	//luaopen_math( m_luaState );
	luaopen_string( m_luaState );

    switch( runScript( "print('Lua: Created LuaVM')" ) )
    {
    case LuaResult::kError_Load: WV_LOG_ERROR( "Failed to load test1 script" ); break;
    case LuaResult::kError_Run:  WV_LOG_ERROR( "Failed to run test1 script" ); break;
    }

    switch( runScript( "function sum(a, b) print(a); return a+b; end" ) )
    {
    case LuaResult::kError_Load: WV_LOG_ERROR( "Failed to load test2 script" ); break;
    case LuaResult::kError_Run:  WV_LOG_ERROR( "Failed to run test2 script" ); break;
    }

    switch( runGlobalFunction( "sum", 5.0, 6.0 ) )
    {
    case kError_StackMismatch: WV_LOG_ERROR( "Stack mismatch" ); break;
    }
}

wv::LuaVM::~LuaVM()
{
	lua_close( m_luaState );
}

wv::LuaResult wv::LuaVM::runScript( const std::string& _code )
{
    int res = luaL_loadstring( m_luaState, _code.c_str() );

    if( res != LUA_OK )
        return LuaResult::kError_Load;
    
    res = lua_pcall( m_luaState, 0, 0, 0 );
    if( res != LUA_OK )
        return LuaResult::kError_Run;
    
    // If it was executed successfuly we 
    // remove the code from the stack
    lua_pop( m_luaState, lua_gettop( m_luaState ) );
    return LuaResult::kSuccess;
}

wv::LuaResult wv::LuaVM::_loadFunc( const std::string& _funcName )
{
    lua_getglobal( m_luaState, _funcName.c_str() );
    if( !lua_isfunction( m_luaState, -1 ) )
    {
        WV_LOG_ERROR( "global '%s' is not a function", _funcName.c_str() );
        return kError;
    }

    return LuaResult::kSuccess;
}

wv::LuaResult wv::LuaVM::_execFunc()
{
    lua_pcall( m_luaState, 2, 1, 0 );
    double sumval = 0.0;
    if( !lua_isnil( m_luaState, -1 ) )
    {
        sumval = lua_tonumber( m_luaState, -1 );
        lua_pop( m_luaState, 1 );
    }

    printf( "sum=%lf\n", sumval );
    
    return kSuccess;
}
