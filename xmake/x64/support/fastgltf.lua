function main( target, _prefix )
    target:add( "defines", _prefix .. "_SUPPORT_FASTGLTF" )
    target:add( "packages", "fastgltf" )
end