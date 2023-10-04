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
        location "WyvernEngine/source"
        targetdir "bin/Wyvern"

        files { "WyvernEngine/source/**.h","WyvernEngine/source/**.cpp" }

        libdirs
        {
            
        }

        includedirs
        {
            "WyvernEngine/source"
        }

        links { }

    
    project "Sandbox"
    kind "ConsoleApp"
    location "Sandbox/source"
    targetdir "bin/Sandbox"

    files { "Sandbox/source/**.h","Sandbox/source/**.cpp" }

    includedirs{ "WyvernEngine/source/","Sandbox/source" }

    links { "WyvernEngine" }