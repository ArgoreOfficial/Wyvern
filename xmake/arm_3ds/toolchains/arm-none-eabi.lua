
includes "../modules/find_devkitpro.lua"

local DEVKITPRO = find_devkitpro()
local DEVKITARM = path.join( DEVKITPRO, "devkitARM" )
local BINDIR = path.join( DEVKITARM, "bin" )

local ARCH = { 
    "-march=armv6k", 
    "-mtune=mpcore", 
    "-mfloat-abi=hard", 
    "-mtp=soft" 
    }

local CFLAGS = {
    "-g", 
    "-Wall", 
    "-O2", 
    "-mword-relocations", 
    "-ffunction-sections"
}

local CXXFLAGS = {
    "-MMD",
    "-MP",
    "-fno-rtti",
    "-fno-exceptions",
    "-std=gnu++11"
}

local LDFLAGS = {
    "-specs=3dsx.specs", 
    "-g",
    "-Wl,-Map,build/map.map"
}    

toolchain("arm-none-eabi")
    set_kind "standalone"

    set_sdkdir( DEVKITARM )
    set_bindir( BINDIR )

    set_toolset( "cc",  path.join( BINDIR, "arm-none-eabi-g++" ) )
    set_toolset( "cxx", path.join( BINDIR, "arm-none-eabi-g++" ) )
    set_toolset( "ld",  path.join( BINDIR, "arm-none-eabi-g++" ) )
    
    add_defines( "__3DS__ " )
    
    add_cflags  ( CFLAGS, ARCH )
    add_cxxflags( CFLAGS, CXXFLAGS, ARCH )
    add_ldflags ( LDFLAGS, ARCH )
        
toolchain_end()
