
project "Chimera"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    location "../../build"
    targetdir "../../bin"
    objdir "../../bin/obj/"

    includedirs { 
        "Wyvern/",
        "Chimera/src/",
        "Chimera/libs/glad/include/", 
        "Chimera/libs/glfw/include/", 
        "Chimera/libs/glm/",
    }

    files { 
        "Chimera/src/**.h", 
        "Chimera/src/**.cpp" 
    }

    links { "GLFW", "GLM", "GLAD" }

    filter "system:linux"
        links { "dl", "pthread" }
        defines { "_X11" }

    filter "system:windows"
        defines { "_WINDOWS" }


group "Engine/Dependencies"

include "Chimera/libs/glfw.lua"
include "Chimera/libs/glad.lua"
include "Chimera/libs/glm.lua"
