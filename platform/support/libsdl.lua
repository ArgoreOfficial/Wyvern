function main(target)
    target:add( "defines", "WV_SDL_SUPPORTED" )

    if target:is_arch( "x86" ) and os.arch() == "x64" then
        target:add( "includedirs", "./libs/SDL2/i686-w64-mingw32/include" )
        target:add( "linkdirs", "./libs/SDL2/i686-w64-mingw32/lib/" )
        target:add( "links", "mingw32", "SDL2main", "SDL2" )
    else
        target:add( "packages", "libsdl" )
    end
end