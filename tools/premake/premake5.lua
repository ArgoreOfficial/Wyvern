
workspace "Wyvern"
	configurations { "Debug", "Release" }
	platforms { "x64", "x86" }
	location "../../"
	debugdir "../../"

	startproject "Wyvern"

group "App"

include "game.lua"

group "Tools"
include "editor.lua"

group "Engine"

include "wv.lua"
include "cm.lua"
include "../../libs/imgui.lua"
