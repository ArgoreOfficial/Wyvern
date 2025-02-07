#include "wv.h"

#include <wv/Debug/Print.h>

static std::string formatLuaArgs( lua_State* _L, int _nargs ) {
    std::string format = "";
    for( int i = 1; i <= _nargs; i++ )
    {
        if( lua_isstring( _L, i ) )
            format += lua_tostring( _L, i );
        else
            format += " ? ";
    }
    return format;
}

int l_wv_log( lua_State* _L )
{
    int nargs = lua_gettop( _L );
    std::string format = formatLuaArgs( _L, nargs );
    wv::Debug::Print( wv::Debug::WV_PRINT_LUA, "%s\n", format.c_str() );

    return 0;
}

int luaopen_wv_global( lua_State* _L )
{

    return 0;
}

int l_wv_logDebug( lua_State* _L )
{
    int nargs = lua_gettop( _L );
    std::string format = formatLuaArgs( _L, nargs );
    wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "%s\n", format.c_str() );

    return 0;
}

int luaopen_wv( lua_State* _L )
{
    luaL_newlib( _L, lualib_wv );

    // override print 
    lua_getglobal( _L, "_G" );
    luaL_setfuncs( _L, lualib_wvPrint, 0 );
    lua_pop( _L, 1 );

    return 1;
}