--[[

    Copyright (C) 2023-2024 Argore 

]]--

PROJECT_NAME = "Sandbox"

set_project( PROJECT_NAME )
set_version( "0.0.1" )

set_symbols "debug"
set_optimize "none"


toolchain( "i686-w64-mingw32" )
    set_kind("standalone")
    set_sdkdir( "C:/msys64/mingw32/" )
    set_bindir( "C:/msys64/mingw32/bin/" )

    set_toolset( "cc",    "i686-w64-mingw32-gcc" )
    set_toolset( "cxx",   "i686-w64-mingw32-g++", "i686-w64-mingw32-gcc" )
    set_toolset( "ld",    "i686-w64-mingw32-g++", "i686-w64-mingw32-gcc" )
    
    --set_toolset( "sh",    "i686-w64-mingw32-g++", "i686-w64-mingw32-gcc" )
    set_toolset( "ar",    "ar" )
    set_toolset( "ex",    "ar" )
    set_toolset( "strip", "strip" )
    --set_toolset( "mm",    "i686-w64-mingw32-gcc" )
    --set_toolset( "mxx",   "i686-w64-mingw32-gcc", "i686-w64-mingw32-g++" )
    --set_toolset( "as",    "i686-w64-mingw32-gcc" )

toolchain_end()

set_toolchains( "i686-w64-mingw32" )

set_languages( "c17", "cxx20" )
set_allowedarchs( "x64", "x86", "x86_64", "wasm32" )

includes( "platform/platforms.lua" )
load_platform()

includes( "source/Engine" )
includes( "source/App" )