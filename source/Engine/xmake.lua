--[[

    Copyright (C) 2023-2024 Argore 

]]--

-- load platform dependencies and other specifics

includes( "../../libs/glad" )

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

-- add_requires("miniaudio") -- TODO: move

-- create project
target "Wyvern" 
    set_kind "static"
    
    if not is_mode("Package") then
        set_basename("Wyvern_$(mode)_$(arch)")
    end

    set_targetdir "../../build"
    
    set_objectdir "../../build/obj"
    set_runtimes "MD" 

    add_headerfiles( 
        "**.h", 
        "**.hpp" 
    )
    add_files( "**.cpp" )
    add_includedirs( "./" )
    
    target_platform()
target_end()