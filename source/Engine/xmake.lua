--[[

    Copyright (C) 2023-2024 Argore 

]]--

-- load platform dependencies and other specifics

if not is_arch( "psvita" ) then
    includes( "../../libs/glad" )
end

local has_vitasdk = os.getenv("SCE_PSP2_SDK_DIR") ~= nil

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
    
    if is_arch( "psvita" ) and has_vitasdk then 
        add_rules( "vitaCg" )
        add_files( "../../game/res/shaders/**.cg" )
    end

    target_platform()
target_end()