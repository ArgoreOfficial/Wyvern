target("Game")
    set_kind "binary"
    add_deps("Wyvern")

    set_warnings( "extra" )

    if is_mode("Package") then
        set_basename("Game_$(arch)")
    else
        set_basename("Game_$(mode)_$(arch)")
    end
    
    add_headerfiles("**.h")
    add_files("**.cpp")
    add_includedirs(
        "./",
        "../engine/"
    )

    add_packages(
        "tracy",
        "vulkan-headers", 
        "volk", 
        "vulkan-memory-allocator" 
    )
target_end()

