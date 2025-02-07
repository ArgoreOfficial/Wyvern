#include "LuaVM.h"

#include <wv/Scripting/Lua/Libraries/wv.h>

#include <wv/Debug/Print.h>
#include <wv/Engine/Engine.h>
#include <wv/Memory/FileSystem.h>

void luaWarnFunc( void* ud, const char* msg, int tocont ) {
    WV_LOG_WARNING( "Lua: %s", msg );
}

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

    luaL_openlibs( m_luaState );

    addLibrary( "wv", luaopen_wv );
    _loadLibs( ~0, 0 );
    
    std::string wv_test = cEngine::get()->m_pFileSystem->loadString( "wv_test.lua" );

    //switch( runScript( "function sum(a, b) print(a); return a+b; end" ) )
    switch( runScript( wv_test ) )
    {
    case LuaResult::kError_Load: WV_LOG_ERROR( "Failed to load wv_test script\n" ); break;
    case LuaResult::kError_Run:  WV_LOG_ERROR( "Failed to run wv_test script\n" ); break;
    }

    {
        LuaStackGuard stackGuard{ m_luaState };

        if( runGlobalFunction( "sum", 5.0, 6.0 ) == kSuccess )
        {
            double sumval = popReturnNumber();
            printf( "sum=%lf\n", sumval );
        }

        if( !stackGuard.check() )
            WV_LOG_ERROR( "Stack mismatch\n" );
    }
}

wv::LuaVM::~LuaVM()
{
	lua_close( m_luaState );
}

void wv::LuaVM::addLibrary( const char* _name, lua_CFunction _loadfunc )
{
    luaL_Reg reg;
    reg.name = _name;
    reg.func = _loadfunc;
    m_libs.push_back( reg );
}

wv::LuaResult wv::LuaVM::runScript( const std::string& _code )
{
    int res = luaL_loadstring( m_luaState, _code.c_str() );

    if( res != LUA_OK )
        return LuaResult::kError_Load;
    
    LuaResult ret = LuaResult::kSuccess;
    res = _pcall( 0, 0, 0 );
    if( res != LUA_OK )
        ret = LuaResult::kError_Run;
    
    lua_pop( m_luaState, lua_gettop( m_luaState ) );
    return ret;
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

void wv::LuaVM::_loadLibs( int _load, int _preload )
{
    for( auto& lib : m_libs )
    {
        luaL_requiref( m_luaState, lib.name, lib.func, 1 );
        lua_pop( m_luaState, 1 );
    }
}

wv::LuaResult wv::LuaVM::_loadFunc( const std::string& _funcName )
{
    lua_getglobal( m_luaState, _funcName.c_str() );
    if( !lua_isfunction( m_luaState, -1 ) )
    {
        WV_LOG_ERROR( "global '%s' is not a function\n", _funcName.c_str() );
        return kError;
    }

    return LuaResult::kSuccess;
}

wv::LuaResult wv::LuaVM::_execFunc( int _numArgs )
{
    return _pcall( _numArgs, 1, 0 ) ? LuaResult::kError : LuaResult::kSuccess;
}

int wv::LuaVM::_pcall( int _nargs, int _nret, int _hpos )
{
    int res = lua_pcall( m_luaState, _nargs, _nret, _hpos );
    if( res && lua_gettop( m_luaState ) )
    {
        const char* err = lua_tostring( m_luaState, -1 );
        printf( "%s\n", err );
        lua_pop( m_luaState, 1 );
    }
    return res;
}
