
--[[

    Copyright (C) 2023-2024 Argore 

]]--

PROJECT_NAME = "Wyvern"
ASSIMP_STATIC_ROOT = "D:/SDK/assimp-static/"

-- files need to be added to this for Emscripten to preload them into the WASM build
WASM_PRELOAD_FILES = {
    "res/shaders/deferred.wshader",
    "res/shaders/phong.wshader",
    "res/shaders/sky.wshader",
    "res/shaders/unlit.wshader",
    
    "res/shaders/source/unlit_vs.glsl",
    "res/shaders/source/deferred_fs.glsl",
    "res/shaders/source/deferred_vs.glsl",
    "res/shaders/source/phong_fs.glsl",
    "res/shaders/source/phong_vs.glsl",
    "res/shaders/source/sky_fs.glsl",
    "res/shaders/source/sky_vs.glsl",
    "res/shaders/source/unlit_fs.glsl",

    "res/meshes/skysphere.glb.wpr",
    "res/meshes/monke.glb.wpr",

    "res/materials/phong.wmat",
    "res/materials/defaultSky.wmat",

    "res/textures/uv.png",
    "res/textures/autumn_field_puresky.png"
}

set_project( PROJECT_NAME )
set_version( "0.0.1" )

set_symbols "debug"
set_optimize "none"

set_languages( "c17", "cxx20" )
set_allowedplats( "windows", "wasm" )
set_allowedarchs( "windows|x64", "windows|x86", "wasm|wasm32" )

includes( "source" )