
project "Wyvern"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
    
	location "../../build"
	targetdir "../../bin"
	objdir "../../bin/obj/"

	includedirs {
		"../../source/engine/",
		"../../libs/glad/include/", 
        "../../libs/glfw/include/", 
        "../../libs/glm/",
		"C:/Program Files/Assimp/include"
	}

	libdirs {
		"C:/Program Files/Assimp/lib/x64"
	}

	files { 
        "../../source/engine/wv/**.cpp",
        "../../source/engine/wv/**.h"
    }

	links { "Chimera", "GLFW" }
    