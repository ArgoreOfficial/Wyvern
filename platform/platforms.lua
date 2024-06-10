includes "platform_windows.lua"
includes "platform_wasm.lua"

function load_platform()
    if     is_plat( "windows" ) then load_platform_windows()
    elseif is_plat( "wasm" )    then load_platform_wasm() 
    end
end

function target_platform()
    if is_plat( "windows" ) then on_load(target_platform_windows) end
end

function init_platform( target )
    if target:is_arch( "3DS-ARM" ) then
        target:add( "files", "source/**.pica", {rule = "3ds.picasso"} )
        target:add( "rules", "3ds.package" )
        target:set( "extension", ".elf" )

        target:add( "includedirs", {
            "D:/dev/3ds/include",
            "C:/devkitpro/libctru/include",
            "D:/dev/3ds/build",
            "C:/devkitPro/devkitARM/arm-none-eabi/include"
        })

        target:add( "linkdirs", "C:/devkitPro/libctru/lib" )
        target:add( "links", "citro3d", "citro2d", "ctru", "m" )
    end
end