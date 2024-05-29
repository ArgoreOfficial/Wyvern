project "GLM"
	kind "StaticLib"
	language "C"

	location "../build/%{prj.name}"
	targetdir "../build/%{prj.name}/bin/%{cfg.platform}_%{cfg.buildcfg}"
	objdir "../build/%{prj.name}/obj/%{cfg.platform}_%{cfg.buildcfg}"
	
	includedirs { "../libs/glm/" }

	files
	{
		"../libs/glm/glm/**"
	}
    
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

		defines
		{
			"_GLM_X11"
		}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		defines 
		{ 
			"_GLM_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"