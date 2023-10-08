language "C++"
filter "configurations:Debug"
    symbols "On"
    defines {"WYVERN_DEBUG"}

filter "configurations:Release"
    symbols "Off"
    optimize "On"
    defines {"WYVERN_RELEASE"}

workspace "Wyvern"
    architecture "x64"
    startproject "Sandbox"
    configurations { "Debug", "Release" }
    platforms { "Win64"}

    -- engine
    
project "Wyvern"
    kind "StaticLib"
    location "Wyvern/source"
    targetdir "bin/Wyvern"
    staticruntime "on"

    defines { "WYVERN_BUILD_DLL" }

    buildmessage 'Compiling %{file.relpath} to %{cfg.targetdir}'
    postbuildcommands {
        'xcopy /S /Q /Y /F "$(SolutionDir)Libraries/copy-to-build" "$(SolutionDir)bin/Sandbox/"',
    }

    files { "Wyvern/source/**.h", "Wyvern/source/**.cpp" }

    libdirs { "Libraries/glfw/lib-vc2022", "Libraries/glew/lib/Release/x64" }

    includedirs { "Wyvern/source", "Libraries/glfw/include", "Libraries/glew/include", "Libraries/glm" }
    
    links { "glfw3", "opengl32", "glew32" }
    

-- editor

project "Editor"
    kind "ConsoleApp"
    debugenvs { "bin/Wyvern" }
    location "Editor/source"
    targetdir "bin/Editor"

    files { "Editor/source/**.h","Editor/source/**.cpp" }

    includedirs{ "Wyvern/source", "Editor/source", "Libraries/glfw/include", "Libraries/glew/include", "Libraries/glm" }
    links { "Wyvern" }


-- sandbox
    
project "Sandbox"
    kind "ConsoleApp"
    debugenvs { "bin/Wyvern" }
    location "Sandbox/source"
    targetdir "bin/Sandbox"

    files { "Sandbox/source/**.h","Sandbox/source/**.cpp" }

    includedirs{ "Wyvern/source", "Sandbox/source", "Libraries/glfw/include", "Libraries/glew/include", "Libraries/glm" }
    links { "Wyvern" }