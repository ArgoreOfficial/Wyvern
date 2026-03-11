set_project( "Game" )
set_version( "0.0.1" )

--set_allowedmodes("debug", "release", "package")
set_allowedarchs( "x64", "wasm32" )
set_allowedplats( "windows", "wasm" )

-- Configure Modes
if is_mode("Debug") then
    set_symbols "debug"
    set_optimize "none"
    set_strip "none"
    add_defines( "WV_TRACK_MEMORY" )
elseif is_mode("Release") then 
    set_symbols "debug"
    set_optimize "fast"
    set_strip "debug"
elseif is_mode("Package") then 
    set_symbols "none"
    set_optimize "fastest"
    set_strip "all"
end

if is_plat("windows") then 
    set_languages( "c17", "cxx20" )
    
    -- Platform Defines
    add_defines( "WV_X64", "WV_ARCH_X64", "WV_PLATFORM_WINDOWS" )

    -- Language Defines
    add_defines( "WV_C17", "WV_CPP20" )

    -- Support Defines
    add_defines( 
        "WV_SUPPORT_VULKAN",
        "WV_SUPPORT_OPENGL",
        "WV_SUPPORT_OPENGLES",
        "WV_SUPPORT_FASTGLTF",
        "WV_SUPPORT_SDL", "WV_SUPPORT_SDL3",
        "WV_SUPPORT_IMGUI",
        "WV_SUPPORT_TRACY",
        "WV_ENABLE_SIMD"
    )
    
    -- Flags
    add_defines( 
        -- "TRACY_ENABLE",
        "VK_NO_PROTOTYPES", 
        "IMGUI_IMPL_VULKAN_USE_VOLK", 
        "IMGUI_IMPL_VULKAN_USE_LOADER" 
    )
    
    -- Package Requires
    -- TODO: check for availability
    add_requires( 
        "fastgltf",
        "libsdl3",
        "tracy",
        "vulkan-headers", 
        "volk", 
        "vulkan-memory-allocator" 
    )
end

includes( "src/engine" )
includes( "src/game" )
