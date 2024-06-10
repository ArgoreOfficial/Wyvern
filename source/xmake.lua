--[[

    Copyright (C) 2023-2024 Argore 

]]--

includes( "../platform/platforms.lua" )
includes( "../libs/glad" )

-- configure modes
if is_mode("Debug") then
    add_defines("WV_DEBUG")
    set_symbols "debug"
    set_optimize "none"
elseif is_mode("Release") then 
    add_defines("WV_RELEASE")
    set_symbols "debug"
    set_optimize "fast"
elseif is_mode("Package") then 
    add_defines("WV_PACKAGE")
    set_symbols "none"
    set_optimize "fastest"
end

-- load platform dependencies and other specifics
load_platform()

-- create project
target( PROJECT_NAME )
    set_kind "binary"
    
    set_targetdir "../game"
    set_objectdir "../build/obj"
    set_runtimes( "MD" )

    add_headerfiles( 
        "Engine/**.h", 
        "Engine/**.hpp" 
    )
    add_files( "Engine/**.cpp" )
    add_includedirs( "Engine" )

    target_platform()
target_end()