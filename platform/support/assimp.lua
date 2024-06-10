local ASSIMP_STATIC_ROOT = "D:/SDK/assimp-static/"

function main(target)
    target:add( "defines", "WV_ASSIMP_SUPPORTED" )
    target:add( "includedirs", ASSIMP_STATIC_ROOT .. "include" )
    target:add( "linkdirs", ASSIMP_STATIC_ROOT .. "lib" )
    target:add( "links", "assimp-vc143-mt.lib", "zlibstatic.lib" )
end