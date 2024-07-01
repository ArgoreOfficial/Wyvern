function main(target)
    target:add( "defines", "WV_SDL_SUPPORTED" )

    -- TODO: change x86 to winxp-x86
    if target:is_arch( "x86" ) and is_mode("Debug-WinXP")then
        target:add( "includedirs", "./libs/SDL2/i686-w64-mingw32/include" )
        target:add( "linkdirs", "./libs/SDL2/i686-w64-mingw32/lib/" )
        target:add( "links", "mingw32", "SDL2main", "SDL2" )
    else
        target:add( "packages", "libsdl" )
    end
end