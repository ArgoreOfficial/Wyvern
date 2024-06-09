
--[[

    Custom GLAD Installation:
        OpenGL
        OpenGL ES1
        OpenGL ES2

]]--

target "GLAD" 
    set_group( "Dependencies" )
    
    set_kind "static"
	add_files "src/glad.c"
    add_includedirs( "include/", {public = true} )
    set_objectdir "../../build/obj"
target_end()