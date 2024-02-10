
project "Game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir "../../bin"
    objdir "../../bin/obj/"
    
    location "../../build"

    includedirs { 
        "../../source/engine/"
    }

    files { 
        "../../source/app/**.h", 
        "../../source/app/**.cpp" 
    }

    links { "Wyvern", "Chimera" }
