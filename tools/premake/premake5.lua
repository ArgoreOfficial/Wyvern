
workspace "Wyvern"
	configurations { "Debug", "Release" }
	platforms { "x64", "x86" }
	location "../../"
	startproject "Game"
	
group "App"

include "../../source/Application/Game.lua"

group "Engine"

include "../../source/Engine/Wyvern.lua"
include "../../source/Engine/Chimera.lua"

