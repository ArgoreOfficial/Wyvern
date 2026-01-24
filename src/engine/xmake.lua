local has_vitasdk = os.getenv("SCE_PSP2_SDK_DIR") ~= nil
local ROOTDIR = "../../"

-- create project
target "Wyvern" 
    set_kind "static"

    set_warnings("extra")

    if not is_mode("Package") then
        set_basename("Wyvern_$(mode)_$(arch)")
    end
    
    if not is_arch("x64") then
        set_prefixname("lib")
    end

    add_headerfiles("**.h", {install = false})
    add_headerfiles("**.hpp", {install = false})   
    
    add_files("**.cpp")
    add_files(
        "**.comp", 
        "**.vert", 
        "**.frag"
    )

    add_includedirs("./")

    add_rules("utils.glsl2spv", {outputdir = "data/shaders/"})

    if is_arch("psvita") and has_vitasdk then 
        add_rules("vitaCg")
        add_files(ROOTDIR .. "data/shaders/**.cg")
    end

    target_platform(ENGINE_NAMESPACE)
target_end()