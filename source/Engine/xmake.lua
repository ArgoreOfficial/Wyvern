--[[

    Copyright (C) 2023-2025 Argore 

]]--

-- load platform dependencies and other specifics

if not is_arch( "psvita" ) then
    includes( "../../libs/glad" )
end

local has_vitasdk = os.getenv("SCE_PSP2_SDK_DIR") ~= nil
local ROOTDIR = "../../"

-- create project
target "Wyvern" 
    set_kind "static"

    set_warnings( "extra" )

    if not is_mode("Package") then
        set_basename("Wyvern_$(mode)_$(arch)")
    end
    
    if not is_arch( "x64" ) then
        set_prefixname("lib")
    end

    -- set_targetdir "../../build"
    set_targetdir( ROOTDIR .. "game/$(plat)" )
    set_objectdir( ROOTDIR .. "build/obj" )

    add_headerfiles( 
        "**.h", 
        "**.hpp",
        ROOTDIR .. "libs/libWyvern/include/**.hpp"
    )
    
    add_files( "**.cpp" )

    add_filegroups("wv", { rootdir = ROOTDIR .. "libs/libWyvern/include/wv/" } )

    add_includedirs( 
        "./",
        ROOTDIR .. "libs/libWyvern/include"
    )
    
    if is_arch( "psvita" ) and has_vitasdk then 
        add_rules( "vitaCg" )
        add_files( ROOTDIR .. "game/shaders/**.cg" )
    end

    target_platform()
target_end()