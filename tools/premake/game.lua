
project "Game"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir "../../bin"
    objdir "../../bin/obj/"
    
    location "../../build"

    includedirs { 
        "../../source/engine/",
		"../../libs/glad/include/", 
        "../../libs/glfw/include/", 
        "../../libs/glm/",
        "C:/Program Files/Assimp/include"
    }
    
    files { 
        "../../source/app/**.h", 
        "../../source/app/**.cpp" 
    }