--[[

    Copyright (C) 2023-2024 Argore 

]]--

PROJECT_NAME = "Sandbox"

set_project( PROJECT_NAME )
set_version( "0.0.1" )

set_symbols "debug"
set_optimize "none"

add_rules("plugin.vsxmake.autoupdate")

if is_mode( "Package" ) then
    set_runtimes "MT"
else
    set_runtimes "MTd"
end

set_allowedarchs( "x64", "x86", "x86_64", "wasm32", "psvita", "3ds-arm" )

includes( "platform/platforms.lua" )
load_platform()

includes( "source/Engine" )
includes( "source/App" )