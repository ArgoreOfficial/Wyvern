includes "platform_3ds.lua"
includes "platform_windows.lua"
includes "platform_wasm.lua"
includes "platform_linux.lua"

includes "toolchains/i686-w64-mingw32.lua"

local PLATFORMS = {
    { plat="windows", arch={ "x64", "x86" }, load=load_platform_windows, target=target_platform_windows },
    { plat="linux",   arch={ "x86_64"     }, load=load_platform_linux,   target=target_platform_linux   },
    { plat="wasm",    arch={ "wasm32"     }, load=load_platform_wasm,    target=target_platform_wasm    }
}

function get_plat()

end

function load_platform()

    -- x86 compiler for w64
    if is_arch( "x86" ) and os.arch() == "x64" then
        set_toolchains( "i686-w64-mingw32" )
    end
    
    -- configure modes
    if is_mode("Debug") then
        set_symbols "debug"
        set_optimize "none"
        set_strip "none"
    elseif is_mode("Release") then 
        set_symbols "debug"
        set_optimize "fast"
        set_strip "debug"
    elseif is_mode("Package") then 
        set_symbols "none"
        set_optimize "fastest"
        set_strip "all"
    end

    for i=1,#PLATFORMS do 
        if is_plat(PLATFORMS[i].plat) and is_arch(table.unpack(PLATFORMS[i].arch)) then
            PLATFORMS[i].load()
        end
    end

    -- configure defines for each platform
    after_load(function (target)
        import( "core.project.config" )
        import("core.base.option")

        target:add( "defines", "WV_PLATFORM_" .. string.upper( config.plat() ) )
        target:add( "defines", "WV_ARCH_"     .. string.upper( config.arch() ) )
        target:add( "defines", "WV_"          .. string.upper( config.mode() ) )

        if option.get("verbose") then
            print( target:name() )
            print( target:get("defines") )
        end
    end)
end


function target_platform(_root)
    for i=1,#PLATFORMS do 
        if is_plat(PLATFORMS[i].plat) and is_arch(table.unpack(PLATFORMS[i].arch)) then 
            on_load(PLATFORMS[i].target)
        end
    end
end


-- 3ds
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
