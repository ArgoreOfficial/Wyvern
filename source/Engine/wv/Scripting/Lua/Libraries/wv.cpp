#include "wv.h"

#include <wv/Debug/Print.h>
#include <string>

static std::string formatLuaArgs( lua_State* _luaState, int _nargs ) {
    std::string format("");

    for( int i = 1; i <= _nargs; i++ )
    {
        if( lua_isstring( _luaState, i ) )
            format += lua_tostring( _luaState, i );
        else
            format += " ? ";
    }
    return format;
}

int l_wv_log( lua_State* _luaState )
{
    int nargs = lua_gettop( _luaState );
    std::string format = formatLuaArgs( _luaState, nargs );
    wv::Debug::Print( wv::Debug::WV_PRINT_LUA, "%s\n", format.c_str() );

    return 0;
}

int luaopen_wv_global( lua_State* _luaState )
{

    return 0;
}

int l_wv_logDebug( lua_State* _luaState )
{
    int nargs = lua_gettop( _luaState );
    std::string format = formatLuaArgs( _luaState, nargs );
    wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "%s\n", format.c_str() );

    return 0;
}

int luaopen_wv( lua_State* _luaState )
{
    luaL_newlib( _luaState, lualib_wv );

    // override print 
    lua_getglobal( _luaState, "_G" );
    luaL_setfuncs( _luaState, lualib_wvPrint, 0 );
    lua_pop( _luaState, 1 );

    return 1;
}