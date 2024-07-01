--[[

    Copyright (C) 2023-2024 Argore 

]]--

-- create project
target( PROJECT_NAME )
    set_kind "binary"
    add_deps "Wyvern"
    
    if not is_mode("Package") then
        set_basename(PROJECT_NAME .. "_$(mode)_$(arch)")
    end

    set_targetdir "../../game"
    set_objectdir "../../build/obj"
    set_runtimes "MT"

    add_headerfiles( 
        "**.h", 
        "**.hpp" 
    )
    add_files( "**.cpp" )
    add_includedirs( "../Engine", "./" )

    target_platform()
target_end()