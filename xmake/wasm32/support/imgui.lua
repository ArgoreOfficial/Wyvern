function main( target, _prefix )
    target:add( "defines", _prefix .. "_SUPPORT_IMGUI" )
    target:add( "packages", "imgui" )
end