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

    
    add_headerfiles( 
        "**.h", 
        "**.hpp"
    )
    
    add_files( "**.cpp" )

    -- add_filegroups("wv", { rootdir = ROOTDIR .. "libs/libWyvern/include/wv/" } )

    add_includedirs("./")
    
    if is_arch( "psvita" ) and has_vitasdk then 
        add_rules( "vitaCg" )
        add_files( ROOTDIR .. "game/shaders/**.cg" )
    end

    target_platform(ENGINE_NAMESPACE)
target_end()