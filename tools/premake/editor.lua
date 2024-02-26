
project "Editor"
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
        "../../libs/imgui/",
        "C:/Program Files/Assimp/include"
    }

    files { 
        "../../source/editor/**.h", 
        "../../source/editor/**.cpp" 
    }
