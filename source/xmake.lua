--[[

    Main Wyvern Project

]]--

includes( "platform.lua" )
includes( "../libs/glad" )

add_requires( "glm" )
add_requires( "libsdl" )

---------- I can't figure out how to get these to work in platform.lua, so they're here for now

if is_mode("Debug") then
    set_symbols "debug"
    set_optimize "none"
end

if is_plat("windows") then add_defines("WV_PLATFORM_WINDOWS") end
if is_plat("wasm") then    add_defines("WV_PLATFORM_WASM") end

-- configuration defines

if is_mode("Debug")   then add_defines("WV_DEBUG") end
if is_mode("Release") then add_defines("WV_RELEASE") end
if is_mode("Package") then add_defines("WV_PACKAGE") end

-- dependencies

add_requires( "glm" )
add_requires( "libsdl" )

if glfw_supported() then 
    add_requires( "glfw" )
end

if is_plat( "wasm" ) then 
    for i=1, #WASM_PRELOAD_FILES do 
        add_values("wasm.preloadfiles", "game/" .. WASM_PRELOAD_FILES[ i ] .. "@" .. WASM_PRELOAD_FILES[ i ] )
    end
    
    add_ldflags( "-sALLOW_MEMORY_GROWTH" )
    add_ldflags( "-sMAX_WEBGL_VERSION=2" )
    add_ldflags( "-sFULL_ES3=1" )
end

target( PROJECT_NAME )
    set_kind "binary"
    
    set_targetdir "../game"
    set_objectdir "../build/obj"
    set_runtimes( "MD" )


    add_packages("glfw", "glm", "libsdl")
    add_deps("GLAD")

    add_headerfiles( 
        "Engine/**.h", 
        "Engine/**.hpp" 
    )
    add_files( "Engine/**.cpp" )
    add_includedirs( "Engine" )

    if is_plat( "windows" ) then 
        add_files( "../resources/resource.rc" )
        add_filegroups( "Resources", { rootdir = "../resources" } )
    end

    if glfw_supported() then 
        add_defines( "WV_GLFW_SUPPORTED" )

        add_includedirs( ASSIMP_STATIC_ROOT .. "include" )

        add_linkdirs( ASSIMP_STATIC_ROOT .. "lib" )
        add_links( "assimp-vc143-mt.lib", "zlibstatic.lib" )
    end
target_end()