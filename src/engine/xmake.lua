local has_vitasdk   = os.getenv("SCE_PSP2_SDK_DIR") ~= nil
local has_vulkansdk = os.getenv("VULKAN_SDK") ~= nil

local ROOTDIR = "../../"

-- create project
target "Wyvern" 
    set_kind "static"
    
    if not is_arch("x64") then
        set_prefixname("lib")
    end

    add_includedirs("./")
    add_headerfiles("**.h", "**.hpp")
    add_headerfiles()
    add_files("**.cpp")

    add_packages("fastgltf", "libsdl3")

    if not is_mode("package") then 
        add_packages("tracy")
    end

    if has_vulkansdk then -- Vulkan Shaders
        add_rules("utils.glsl2spv", {outputdir = "data/shaders/"})
        add_files("**.comp", "**.vert", "**.frag")

        add_packages(
            "vulkan-headers",
            "volk",
            "vulkan-memory-allocator"
        )
    end

    if is_arch("psvita") and has_vitasdk then 
        add_rules("vitaCg")
        add_files(ROOTDIR .. "data/shaders/**.cg")
    end
target_end()