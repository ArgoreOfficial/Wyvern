--[[

    Copyright (C) 2023-2024 Argore 

]]--

PROJECT_NAME = "Sandbox"

set_project( PROJECT_NAME )
set_version( "0.0.1" )

set_symbols "debug"
set_optimize "none"

if is_mode( "Package" ) then
    set_runtimes "MT"
else
    set_runtimes "MTd"
end

set_allowedarchs( "x64", "x86", "x86_64", "wasm32", "psvita", "arm_3ds" )

includes( "platform/platforms.lua" )
init_platform()

includes( "source/Engine" )
includes( "source/App" )