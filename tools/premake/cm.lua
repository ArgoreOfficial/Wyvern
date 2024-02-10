
project "Chimera"
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
        "../../libs/glm/"
    }

    files { 
        "../../source/engine/cm/**.h", 
        "../../source/engine/cm/**.cpp"
    }

    links { "GLFW", "GLM", "GLAD" }

group "Engine/Dependencies"

include "../../libs/glfw.lua"
include "../../libs/glad.lua"
include "../../libs/glm.lua"
