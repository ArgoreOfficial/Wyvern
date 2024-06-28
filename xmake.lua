--[[

    Copyright (C) 2023-2024 Argore 

]]--

PROJECT_NAME = "Sandbox"

set_project( PROJECT_NAME )
set_version( "0.0.1" )

set_symbols "debug"
set_optimize "none"

set_languages( "c17", "cxx20" )
set_allowedarchs( "x64", "x86", "x86_64", "wasm32" )

includes( "platform/platforms.lua" )
load_platform()

includes( "source/Engine" )
includes( "source/App" )