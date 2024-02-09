project "Wyvern"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
    
	location "../../build"
	targetdir "../../bin"
	objdir "../../bin/obj/"
	
	includedirs {
		"./Chimera/src"
	}

	files { 
        "Wyvern/src/*.cpp",
        "Wyvern/src/*.h",
    }

	links { "Chimera" }
    