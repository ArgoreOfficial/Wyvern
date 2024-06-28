includes "platform_3ds.lua"
includes "platform_windows.lua"
includes "platform_wasm.lua"
includes "platform_linux.lua"

local PLATFORMS = {
    { plat="windows", arch={ "x64", "x86" }, load=load_platform_windows, target=target_platform_windows },
    { plat="linux",   arch={ "x64_86"     }, load=load_platform_linux,   target=target_platform_linux   },
    { plat="wasm",    arch={ "wasm32"     }, load=load_platform_wasm,    target=target_platform_wasm    }
}

function load_platform()
    for i=1,#PLATFORMS do 
        if is_plat(PLATFORMS[i].plat) and is_arch(table.unpack(PLATFORMS[i].arch)) then 
            PLATFORMS[i].load()
            -- printf( "Loading %s:%s\n", PLATFORMS[i].plat, table.concat(PLATFORMS[i].arch, ",") )
        end
    end
end

function target_platform(_root)
    for i=1,#PLATFORMS do 
        if is_plat(PLATFORMS[i].plat) and is_arch(table.unpack(PLATFORMS[i].arch)) then 
            on_load(PLATFORMS[i].target)
            -- printf( "Targetting %s:%s\n", PLATFORMS[i].plat, table.concat(PLATFORMS[i].arch, ",") )
        end
    end
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