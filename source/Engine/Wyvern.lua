
project "Wyvern"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
    
	location "../../build"
	targetdir "../../bin"
	objdir "../../bin/obj/"
	
	includedirs {
		"Wyvern/",
		"Chimera/src",
		"Chimera/libs/glad/include/", 
        "Chimera/libs/glfw/include/", 
        "Chimera/libs/glm/",
	}

	files { 
        "Wyvern/**.cpp",
        "Wyvern/**.h",
    }

	links { "Chimera", "GLFW" }
    