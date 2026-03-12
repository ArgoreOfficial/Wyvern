local has_vulkansdk = os.getenv("VULKAN_SDK") ~= nil

target("Game")
    set_kind("binary")
    add_deps("Wyvern")
        
    add_headerfiles("**.h")
    add_files("**.cpp")
    add_includedirs("./", "../engine/")
    add_packages("tracy")

    if has_vulkansdk then 
        add_packages(
            "vulkan-headers", 
            "volk", 
            "vulkan-memory-allocator"
        )
    end
target_end()

