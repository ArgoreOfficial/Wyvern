
project "Wyvern"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
    
	location "../../build"
	targetdir "../../bin"
	objdir "../../bin/obj/"

	includedirs {
		"../../source/app/",
		"../../source/editor/",
		"../../source/engine/",
		"../../libs/glad/include/", 
        "../../libs/glfw/include/", 
        "../../libs/glm/",
        "../../libs/imgui/",
		"C:/Program Files/Assimp/include"
	}

	libdirs {
		"C:/Program Files/Assimp/lib/x64"
	}

	files { 
        "../../source/engine/wv/**.cpp",
        "../../source/engine/wv/**.h"
    }

	links { "Game", "Editor", "Chimera", "GLFW", "assimp-vc143-mt" }
    
	filter "Debug"
  		defines { "WV_DEBUG" }
	
	filter "Release"
		defines { "WV_RELEASE" }