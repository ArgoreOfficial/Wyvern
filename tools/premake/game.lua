
project "Game"
    kind "ConsoleApp"
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

    libdirs {
		"C:/Program Files/Assimp/lib/x64"
	}

    files { 
        "../../source/app/**.h", 
        "../../source/app/**.cpp" 
    }

    links { "Wyvern", "Chimera", "assimp-vc143-mt" }
