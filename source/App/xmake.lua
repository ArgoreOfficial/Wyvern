--[[

    Copyright (C) 2023-2024 Argore 

]]--

-- load platform dependencies and other specifics

-- configure modes
if is_mode("Debug") then
    add_defines("WV_DEBUG")
    set_symbols "debug"
    set_optimize "none"
    set_strip "none"
elseif is_mode("Release") then 
    add_defines("WV_RELEASE")
    set_symbols "debug"
    set_optimize "fast"
    set_strip "debug"
elseif is_mode("Package") then 
    add_defines("WV_PACKAGE")
    set_symbols "none"
    set_optimize "fastest"
    set_strip "all"
end

-- create project
target( PROJECT_NAME )
    set_kind "binary"
    add_deps "Wyvern"
    
    if not is_mode("Package") then
        set_basename(PROJECT_NAME .. "_$(mode)_$(arch)")
    end

    set_targetdir "../../game"
    set_objectdir "../../build/obj"
    set_runtimes "MD"

    add_headerfiles( 
        "**.h", 
        "**.hpp" 
    )
    add_files( "**.cpp" )
    add_includedirs( "../Engine", "./" )

    target_platform()
target_end()