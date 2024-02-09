local path = require "./path"
PROJECT_NAME = path.getProjectName( 2 )

workspace (PROJECT_NAME)
	configurations { "Debug", "Release" }
	platforms { "x64", "x86" }
	location "../../"

project (PROJECT_NAME)
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	
	targetdir "../../bin"
	objdir "../../bin/obj/"

	location "../../build"

	files { 
		"../../src/**.h", 
		"../../src/**.cpp" 
	}
