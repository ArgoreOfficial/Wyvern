
workspace "Wyvern"
	configurations { "Debug", "Release" }
	platforms { "x64", "x86" }
	location "../../"
	startproject "Game"
	debugdir "../../"
	
	
group "App"

include "game.lua"

group "Engine"

include "wv.lua"
include "cm.lua"

