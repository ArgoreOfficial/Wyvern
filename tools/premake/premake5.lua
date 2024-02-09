
workspace "Wyvern"
	configurations { "Debug", "Release" }
	platforms { "x64", "x86" }
	location "../../"

group "Engine"

include "../../source/Engine/Wyvern.lua"
include "../../source/Engine/Chimera.lua"
