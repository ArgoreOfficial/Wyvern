
------------------ Config ------------------
PROJECT_NAME = "Wyvern"
ASSIMP_STATIC_ROOT = "D:/SDK/assimp-static/"
--------------------------------------------

set_languages( "c17", "cxx20" )

target "GLAD" 
    set_kind "static"
    add_includedirs "libs/glad/include/"
	add_files "libs/glad/src/glad.c"
    add_defines( "_GLAD_WIN32", "_CRT_SECURE_NO_WARNINGS" )
target_end()

--------------------------------------------------------------------------

target "GLM" 
    set_kind "static"
    add_includedirs "libs/glm/"

    add_headerfiles( "libs/glm/glm/**.h" )
    add_headerfiles( "libs/glm/glm/**.hpp" )
	add_files "libs/glm/glm/**.cpp"

    add_defines( "_GLAD_WIN32", "_CRT_SECURE_NO_WARNINGS" )
target_end()

--------------------------------------------------------------------------

add_requires("glfw")

if is_mode("Debug") then
    set_symbols( "debug" )
    set_optimize( "none" )
elseif is_mode( "Release" ) then 
    set_symbols( "debug" )
    set_optimize( "faster" )
elseif is_mode( "Package" ) then 
    set_symbols( "none" )
    set_optimize( "fastest" )
end

target( PROJECT_NAME )
    set_kind "binary"
    set_targetdir "/game"
    set_runtimes( "MD" )

    add_packages("glfw")
    add_deps( "GLM", "GLAD" )

    add_headerfiles "source/Engine/**.h"
    add_headerfiles "source/Engine/**.hpp"
    add_files "source/Engine/**.cpp"
    
    add_includedirs "source/Engine" 

    add_includedirs "libs/glad/include"
    add_includedirs "libs/glfw/include"
    add_includedirs "libs/glm"
    add_includedirs( ASSIMP_STATIC_ROOT .. "include" )

    add_linkdirs( ASSIMP_STATIC_ROOT .. "lib" )
    add_links( "assimp-vc143-mt.lib", "zlibstatic.lib" )
    add_defines( "_WINDOWS" )
target_end()