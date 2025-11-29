PROJECT_NAME = "Game"
ENGINE_NAME  = "Wyvern"
ENGINE_NAMESPACE = "WV"

set_project( PROJECT_NAME )
set_version( "0.0.1" )

set_symbols "debug"
set_optimize "none"

if is_mode( "Package" ) then
    set_runtimes "MT"
else
    set_runtimes "MTd"
end

set_allowedarchs( "x64", "wasm32" )
set_allowedplats( "windows", "wasm" )

includes "xmake/platforms.lua"
init_platform( ENGINE_NAMESPACE )

includes( "src/engine" )
includes( "src/game" )
