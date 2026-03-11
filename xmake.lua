set_project( "Game" )
set_version( "0.0.1" )

set_allowedmodes("debug", "release", "package")
set_allowedarchs( "x64", "wasm32" )
set_allowedplats( "windows", "wasm" )

includes( "src/engine" )
includes( "src/game" )
