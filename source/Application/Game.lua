project "Game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir "../../bin"
    objdir "../../bin/obj/"
    
    location "../../build"

    includedirs { 
        "./",
        "../Engine/Wyvern/",
        "../Engine/Chimera/src/",
    }

    files { 
        "Game/**.h", 
        "Game/**.cpp" 
    }

    links { "Wyvern" }
