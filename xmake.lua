local has_vulkansdk = os.getenv("VULKAN_SDK") ~= nil

set_project( "Game" )
set_version( "0.0.1" )

includes("xmake/rules/mode")
includes("xmake/rules/platform")

set_allowedmodes("debug", "release", "package")
set_allowedarchs( "x64", "wasm32" )
set_allowedplats( "windows", "wasm" )

add_rules("wv.debug", "wv.release", "wv.package")
add_rules("wv.platform.windows")

if is_plat("windows") then 
    -- Support Defines
    add_defines( 
        "WV_SUPPORT_OPENGL",
        "WV_SUPPORT_OPENGLES",
        "WV_SUPPORT_FASTGLTF",
        "WV_SUPPORT_SDL", "WV_SUPPORT_SDL3",
        "WV_SUPPORT_IMGUI",
        "WV_SUPPORT_TRACY"
    )

    if has_vulkansdk then 
        add_defines("WV_SUPPORT_VULKAN")

        add_defines( 
            "VK_NO_PROTOTYPES", 
            "IMGUI_IMPL_VULKAN_USE_VOLK", 
            "IMGUI_IMPL_VULKAN_USE_LOADER" 
        )

        add_requires(
            "vulkan-headers", 
            "volk", 
            "vulkan-memory-allocator"
        )
    end

    
    -- Flags
    -- add_defines( "TRACY_ENABLE" )
    
    -- Package Requires
    add_requires( 
        "fastgltf",
        "libsdl3",
        "tracy"
    )
end

includes( "src/engine" )
includes( "src/game" )
