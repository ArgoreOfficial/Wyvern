-- local ASSIMP_STATIC_ROOT = "D:/SDK/assimp-static/"

function main(target)
    target:add( "defines", "WV_ASSIMP_SUPPORTED" )
    target:add( "packages", "assimp" )
end