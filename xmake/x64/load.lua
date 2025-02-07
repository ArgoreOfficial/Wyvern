--includes "../toolchains/i686-w64-mingw32.lua"  -- win32 toolchain

function load()
    set_languages( "c17", "cxx20" )
    add_defines( "WV_C17", "WV_CPP20" )
    add_defines( "WV_PLATFORM_WINDOWS" )

    add_requires( "libsdl", "assimp", "joltphysics" )
    add_requires( "imgui v1.91.0-docking", { 
        configs={
            opengl3 = true,
            glfw    = true,
            sdl2    = true
        }})

    if( is_arch( "x64" ) ) then
        add_requires( "glfw" )
        -- x86 compiler for w64
    elseif is_arch( "x86" ) and os.arch() == "x64" then
        set_toolchains( "i686-w64-mingw32" )
    end
end

PLATFORMS[ "x64" ] = { 
    plat="windows",
    arch={ "x64", "x86"  },
    load=load
}