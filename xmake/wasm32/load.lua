function load(_prefix)
    set_languages( "c17", "cxx20" )
    add_defines( _prefix .. "_C17", _prefix .. "_CPP20" )
    add_defines( _prefix .. "_PLATFORM_WEB" )
    add_defines( _prefix .. "_SUPPORT_OPENGL" )

    add_requires( "libsdl" )
end

PLATFORMS[ "wasm32" ] = { 
    plat="wasm",
    arch={ "wasm32" },
    load=load
}