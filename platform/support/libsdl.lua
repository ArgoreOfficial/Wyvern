function main(target)
    target:add( "defines", "WV_SDL_SUPPORTED" )

    -- TODO: change x86 to winxp-x86
    if target:is_arch( "x86" ) then
        target:add( "includedirs", "./libs/SDL2/i686-w64-mingw32/include" )
        target:add( "linkdirs", "./libs/SDL2/i686-w64-mingw32/lib/" )
        target:add( "links", "mingw32", "SDL2main", "SDL2" )
        --target:add( "defines", "main=SDL_main" )
        --target:add( "ldflags", "-lmingw32 -lSDL2main -lSDL2 -mwindows  -Wl,--dynamicbase -Wl,--nxcompat -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid", {force=true} )
    else
        target:add( "packages", "libsdl" )
    end
end