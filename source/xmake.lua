
target( PROJECT_NAME )
    set_kind "binary"
    
    set_targetdir "../game"
    set_objectdir "../build/obj"
    set_runtimes( "MD" )

    add_packages("glfw", "glm" )
    add_deps( "GLAD" )

    add_headerfiles( "Engine/**.h", "Engine/**.hpp" )
    add_files ( "Engine/**.cpp", "../resources/resource.rc" )
    add_filegroups( "Resources", { rootdir = "../resources" } )

    add_includedirs( "Engine", ASSIMP_STATIC_ROOT .. "include" ) 

    add_linkdirs( ASSIMP_STATIC_ROOT .. "lib" )
    add_links( "assimp-vc143-mt.lib", "zlibstatic.lib" )
target_end()