function main( target, _prefix )
    target:add( "defines", _prefix .. "_SUPPORT_SDL" )
    target:add( "defines", _prefix .. "_SUPPORT_SDL3" )
    target:add( "packages", "libsdl3" )
end