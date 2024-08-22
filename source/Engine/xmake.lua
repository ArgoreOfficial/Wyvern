--[[

    Copyright (C) 2023-2024 Argore 

]]--

-- load platform dependencies and other specifics

if not is_arch( "psvita" ) then
    includes( "../../libs/glad" )
end

-- create project
target "Wyvern" 
    set_kind "static"

    if not is_mode("Package") then
        set_basename("Wyvern_$(mode)_$(arch)")
    end
    
    -- set_targetdir "../../build"
    
    set_objectdir "../../build/obj" 

    add_headerfiles( 
        "**.h", 
        "**.hpp" 
    )
    add_files( "**.cpp" )
    add_includedirs( "./" )
    
    target_platform()
target_end()