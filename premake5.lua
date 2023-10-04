language "C++"
filter "configurations:Debug"
        symbols "On"

filter "configurations:Release"
    symbols "Off"
    optimize "On"

workspace "Wyvern"

    startproject "Game"
    configurations { "Debug","Release" }
    platforms { "Win64"}

    project "WyvernEngine"

        defines {"WYVERN_BUILD_DLL"}

        kind "SharedLib"
        location "WyvernEngine/src"
        targetdir "bin/Wyvern"

        files { "WyvernEngine/src/**.h","WyvernEngine/src/**.cpp" }

        libdirs
        {
            
        }

        includedirs
        {
            "WyvernEngine/src"
        }

        links { }

    
    project "Sandbox"
    kind "ConsoleApp"
    location "Sandbox/src"
    targetdir "bin/Sandbox"

    files { "Sandbox/src/**.h","Sandbox/src/**.cpp" }

    includedirs{ "WyvernEngine/src/","Sandbox/src" }

    links { "WyvernEngine" }