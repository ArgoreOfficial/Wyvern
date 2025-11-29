--includes "../toolchains/i686-w64-mingw32.lua"  -- win32 toolchain

function load(_prefix)
    set_languages( "c17", "cxx20" )
    add_defines( _prefix .. "_C17", _prefix .. "_CPP20" )
    add_defines( _prefix .. "_PLATFORM_WINDOWS" )
    add_defines( _prefix .. "_SUPPORT_OPENGL" )

    add_requires( "libsdl" )
    add_requires( "imgui v1.91.0-docking", { 
        configs={
            opengl3 = true,
            sdl2    = true
        }})    
end

PLATFORMS[ "x64" ] = { 
    plat="windows",
    arch={ "x64", "x86" },
    load=load
}