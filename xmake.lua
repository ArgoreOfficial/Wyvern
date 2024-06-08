
--[[

    Copyright (C) 2023-2024 Argore 

]]--

PROJECT_NAME = "Wyvern"
ASSIMP_STATIC_ROOT = "D:/SDK/assimp-static/"

set_project( PROJECT_NAME )
set_version( "0.0.1" )

set_symbols "debug"
set_optimize "none"

set_languages( "c17", "cxx20" )
--set_allowedplats( "windows", "wasm" )
--set_allowedarchs( "windows|x64", "windows|x86", "wasm|wasm32" )

includes( "source/platform.lua" )
includes( "libs/glad" )
includes( "source" )