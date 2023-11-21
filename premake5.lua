local BUILD_DIR = "build"
local BIN_DIR = "bin"
local GLFW_DIR = "glfw"
local WYVERN_DIR = "Wyvern"
local IMGUI_DIR = "imgui"
local ASSIMP_DIR = "assimp"
local GLAD_DIR = "glad"

language "C++"
cppdialect "C++17"

solution "Wyvern"
	startproject "Sandbox"
	configurations { "Debug", "Release", "Final" }
	targetdir (BIN_DIR)
	
	if os.is64bit() and not os.istarget("windows") then
		platforms "x86_64"
	else
		platforms { "x86", "x86_64" }
	end

	filter "configurations:Debug*"
		defines
		{
			"_DEBUG",
			"WYVERN_DEBUG"
		}
		staticruntime "On"
		runtime "Debug"
		optimize "Debug"
		symbols "On"

	filter "configurations:Release"
		defines 
		{
			"NDEBUG",
			"WYVERN_RELEASE"
		}
		staticruntime "On"
		runtime "Release"
		optimize "On"
		symbols "On"

	filter "configurations:Final*"
		defines 
		{
			"NDEBUG",
			"WYVERN_FINAL"
		}
		staticruntime "On"
		runtime "Release"
		optimize "Debug"
		symbols "Off"

	filter "platforms:x86"
		architecture "x86"
		defines { "WV_PLATFORM_WINDOWS_32" }

	filter "platforms:x86_64"
		architecture "x86_64"
		defines { "WV_PLATFORM_WINDOWS" }

project "Wyvern"
	kind "StaticLib"
	language "C++"
	location (WYVERN_DIR)
	rtti "Off"

	files 
	{ 
		path.join( WYVERN_DIR, "**.h" ),
		path.join( WYVERN_DIR, "**.cpp" ),
	}

	includedirs
	{
		path.join( GLAD_DIR, "include" ),
		path.join( GLFW_DIR, "include" ),
		WYVERN_DIR,
		IMGUI_DIR,
		ASSIMP_DIR
	}

	links { "glfw", "imgui", "assimp/assimp-vc143-mt" }


local PROJECT_NAME = "Sandbox"
local PROJECT_DIR  = "Sandbox"

project (PROJECT_NAME)
	kind "ConsoleApp"
	language "C++"
	location (PROJECT_DIR)
	debugdir (PROJECT_DIR)
	targetdir (BUILD_DIR)
	
	files 
	{ 
		path.join( PROJECT_DIR, "**.h" ),
		path.join( PROJECT_DIR, "**.cpp" ),
	}

	includedirs
	{
		path.join( GLAD_DIR, "include" ),
		path.join( GLFW_DIR, "include" ),
		WYVERN_DIR,
		IMGUI_DIR
	}

	links { "glad", "Wyvern", "gdi32.lib", "kernel32.lib", "psapi" }

project "imgui"
	kind "StaticLib"
	language "C++"
	location (IMGUI_DIR)
	files
	{
		path.join(IMGUI_DIR, "**.h"),
		path.join(IMGUI_DIR, "**.cpp"),
	}
	includedirs
	{
		IMGUI_DIR,
		path.join(GLFW_DIR, "include")
	}
	links { "glfw" }


project "glad"
	kind "StaticLib"
	language "C++"
	location (GLAD_DIR)
	files
	{
		path.join(GLAD_DIR, "**.h"),
		path.join(GLAD_DIR, "**.c"),
		path.join(GLAD_DIR, "**.cpp"),
	}
	includedirs 
	{
		path.join(GLAD_DIR, "include"),
		path.join(GLAD_DIR, "include/glad"),
	}


project "glfw"
	kind "StaticLib"
	language "C"
	location (GLFW_DIR)
	files
	{
		path.join(GLFW_DIR, "include/GLFW/*.h"),
		path.join(GLFW_DIR, "src/context.c"),
		path.join(GLFW_DIR, "src/egl_context.*"),
		path.join(GLFW_DIR, "src/init.c"),
		path.join(GLFW_DIR, "src/input.c"),
		path.join(GLFW_DIR, "src/internal.h"),
		path.join(GLFW_DIR, "src/monitor.c"),
		path.join(GLFW_DIR, "src/null*.*"),
		path.join(GLFW_DIR, "src/osmesa_context.*"),
		path.join(GLFW_DIR, "src/platform.c"),
		path.join(GLFW_DIR, "src/vulkan.c"),
		path.join(GLFW_DIR, "src/window.c"),
	}
	includedirs 
	{
		path.join(GLFW_DIR, "include") 
	}

	filter "system:windows"
		defines "_GLFW_WIN32"
		files
		{
			path.join(GLFW_DIR, "src/win32_*.*"),
			path.join(GLFW_DIR, "src/wgl_context.*")
		}
	filter "system:linux"
		defines "_GLFW_X11"
		files
		{
			path.join(GLFW_DIR, "src/glx_context.*"),
			path.join(GLFW_DIR, "src/linux*.*"),
			path.join(GLFW_DIR, "src/posix*.*"),
			path.join(GLFW_DIR, "src/x11*.*"),
			path.join(GLFW_DIR, "src/xkb*.*")
		}
	filter "system:macosx"
		defines "_GLFW_COCOA"
		files
		{
			path.join(GLFW_DIR, "src/cocoa_*.*"),
			path.join(GLFW_DIR, "src/posix_thread.h"),
			path.join(GLFW_DIR, "src/nsgl_context.h"),
			path.join(GLFW_DIR, "src/egl_context.h"),
			path.join(GLFW_DIR, "src/osmesa_context.h"),

			path.join(GLFW_DIR, "src/posix_thread.c"),
			path.join(GLFW_DIR, "src/nsgl_context.m"),
			path.join(GLFW_DIR, "src/egl_context.c"),
			path.join(GLFW_DIR, "src/nsgl_context.m"),
			path.join(GLFW_DIR, "src/osmesa_context.c"),                       
		}

	filter "action:vs*"
		defines "_CRT_SECURE_NO_WARNINGS"
