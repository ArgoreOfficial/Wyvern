
-- TODO: change to config file
PROJECT_NAME = "Wyvern"

workspace (PROJECT_NAME)
	configurations { "Debug", "Release" }
	platforms { "x64", "x86" }
	location "../../"
	startproject (PROJECT_NAME)

	flags { "MultiProcessorCompile" }

	filter "configurations:Debug"
		defines { "DEBUG", "DEBUG_SHADER" }
		symbols "On"

	filter "configurations:Release"
		defines { "RELEASE" }
		optimize "Speed"
		flags { "LinkTimeOptimization" }

project (PROJECT_NAME)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	targetdir "../../game"
	debugdir "../../game"

	location "../../build/%{prj.name}"
	targetname "%{prj.name}_%{cfg.platform}_%{cfg.buildcfg}"
	objdir "../../build/%{prj.name}/obj/%{cfg.platform}_%{cfg.buildcfg}"


	includedirs { 
		"../../source/Engine",
		"../../include/", 
		"../../libs/glad/include/", 
		"../../libs/glfw/include/", 
		"../../libs/glm/", 
		"../../libs/imgui/", 
		"../../libs/imgui/examples",
		"D:/SDK/assimp-static/include"
	}

	files { 
		"../../source/**.h", 
		"../../source/**.cpp" 
	}
	
	libdirs { "D:/SDK/assimp-static/lib" }

	links { "GLFW", "GLM", "GLAD", "ImGui" }

	filter { "configurations:Debug" }
		links { "assimp-vc143-mtd.lib", "zlibstaticd.lib" }

	filter { "configurations:Release" }
		links { "assimp-vc143-mt.lib", "zlibstatic.lib" }

	filter "system:linux"
		links { "dl", "pthread" }
		defines { "_X11" }

	filter "system:windows"
		defines { "_WINDOWS" }

		files { "../../resources/resource.rc", "../../resources/**.ico" }
		vpaths { ['Resources/*'] = { '*.rc', '**.ico' } }

	filter{}

include "../../libs/glfw.lua"
include "../../libs/glad.lua"
include "../../libs/glm.lua"
include "../../libs/imgui.lua"