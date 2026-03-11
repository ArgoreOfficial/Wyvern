local has_vitasdk = os.getenv("SCE_PSP2_SDK_DIR") ~= nil
local ROOTDIR = "../../"

-- create project
target "Wyvern" 
    set_kind "static"

    set_warnings("extra")
    
    if not is_arch("x64") then
        set_prefixname("lib")
    end

    add_includedirs("./")
    add_headerfiles("**.h", "**.hpp")
    add_headerfiles()
    add_files(
        --"**.comp", "**.vert", "**.frag",
        "**.cpp"
    )

    add_packages(
        "fastgltf",
        "libsdl3",
        "tracy",
        "vulkan-headers", 
        "volk", 
        "vulkan-memory-allocator" 
    )

    -- add_rules("utils.glsl2spv", {outputdir = "data/shaders/"})

    if is_arch("psvita") and has_vitasdk then 
        add_rules("vitaCg")
        add_files(ROOTDIR .. "data/shaders/**.cg")
    end
target_end()