local path = require "./path"
PROJECT_NAME = path.getProjectName( 2 )
TARGET_DIR = "bin/"

rule "glsl_spirv_vert"
  display "GLSL_Spirv Compiler"
  fileextension ".vert"

  buildmessage 'Compiling Vertex %(Filename)'
  buildcommands 'glslc.exe %(FullPath) -o %{wks.location}%{TARGET_DIR}%(Filename)_vert.spv'
  buildoutputs ''

rule "glsl_spirv_frag"
  display "GLSL_Spirv Compiler"
  fileextension ".frag"

  buildmessage 'Compiling Fragment %(Filename)'
  buildcommands 'glslc.exe %(FullPath) -o %{wks.location}%{TARGET_DIR}%(Filename)_frag.spv'
  buildoutputs ''

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

project (PROJECT_NAME)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"

	--rules { "glsl_spirv_frag", "glsl_spirv_vert" }

	targetdir "%{wks.location}%{TARGET_DIR}"
	objdir "%{wks.location}%{TARGET_DIR}obj/"
	location "../../build"
	debugdir "%{wks.location}%{TARGET_DIR}"

	includedirs { 
		"../../include/", 
		"../../libs/glfw/include/", 
		"../../libs/glm/", 
		"$(VULKAN_SDK)/include"
	}

	files { 
		"../../src/**.h", 
		"../../src/**.cpp",
		"../../src/**.vert",
		"../../src/**.frag"
	}

	links { "GLFW", "GLM", "$(VULKAN_SDK)/lib/vulkan-1.lib" }

	filter "system:windows"
		defines { "_WINDOWS" }

include "../../libs/glfw.lua"
include "../../libs/glm.lua"