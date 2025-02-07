#include "LuaVM.h"

extern "C" {
#include <lua/lua.h>
#include <lua/lualib.h>
#include <lua/lauxlib.h>
}

#include <wv/Debug/Print.h>
#include <wv/Engine/Engine.h>
#include <wv/Memory/FileSystem.h>


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
	//luaopen_string( m_luaState );

    std::string wv_test = cEngine::get()->m_pFileSystem->loadString( "wv_test.lua" );

    //switch( runScript( "function sum(a, b) print(a); return a+b; end" ) )
    switch( runScript( wv_test ) )
    {
    case LuaResult::kError_Load: WV_LOG_ERROR( "Failed to load wv_test script" ); break;
    case LuaResult::kError_Run:  WV_LOG_ERROR( "Failed to run wv_test script" ); break;
    }

    {
        LuaStackGuard stackGuard{ m_luaState };

        if( runGlobalFunction( "sum", 5.0, 6.0 ) == kSuccess )
        {
            double sumval = popReturnNumber();
            printf( "sum=%lf\n", sumval );
        }

        if( !stackGuard.check() )
            WV_LOG_ERROR( "Stack mismatch" );
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

double wv::LuaVM::popReturnNumber()
{
    double val = 0.0;
    if( !lua_isnumber( m_luaState, -1 ) )
        WV_LOG_ERROR( "Lua: Incorrect return type. Expected number, got %s\n", lua_typename( m_luaState, lua_type( m_luaState, -1 ) ) );
    else
        val = lua_tonumber( m_luaState, -1 );
    
    lua_pop( m_luaState, 1 );
    return val;
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

wv::LuaResult wv::LuaVM::_execFunc( int _numArgs )
{
    lua_pcall( m_luaState, _numArgs, 1, 0 );
    return kSuccess;
}
