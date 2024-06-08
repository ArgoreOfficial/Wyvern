
-- custom glad installation with OpenGL, GLES1 and GLES2
target "GLAD" 
    set_group( "Dependencies" )
    
    set_kind "static"
	add_files "src/glad.c"
    add_includedirs( "include/", {public = true} )
    set_objectdir "../../build/obj"
target_end()