toolchain( "i686-w64-mingw32" )
    set_kind("standalone")
    set_sdkdir( "C:/msys64/mingw32/" )
    set_bindir( "C:/msys64/mingw32/bin/" )

    set_toolset( "cc",    "i686-w64-mingw32-gcc" )
    set_toolset( "cxx",   "i686-w64-mingw32-g++", "i686-w64-mingw32-gcc" )
    set_toolset( "ld",    "i686-w64-mingw32-g++", "i686-w64-mingw32-gcc" )

    set_toolset( "ar",    "ar" )
    set_toolset( "ex",    "ar" )
    set_toolset( "strip", "strip" )
toolchain_end()