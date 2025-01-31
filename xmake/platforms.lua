
PLATFORMS = {
    -- { plat="3ds",     arch={ "3ds-arm"     }, load=load_platform_3ds,     target=target_platform_3ds     },
    -- { plat="psvita",  arch={ "psvita"      }, load=load_platform_psvita,  target=target_platform_psvita, run=run_platform_psvita  }
}


function init_platform()

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

    for _, filepath in ipairs( os.dirs("xmake/*"), path.translate ) do
        local split = path.split( filepath )
        local arch_name = split[ #split ]

        if is_arch( arch_name ) then
            includes( path.join( filepath, "load.lua" ) )

            if PLATFORMS[ arch_name ].load then
                add_moduledirs( filepath )
                PLATFORMS[ arch_name ].load()
            end
            break
        end
    end

    -- configure defines for each platform
    after_load(function (target)
        import( "core.project.config" )
        import("core.base.option")

        -- target:add( "defines", "WV_PLATFORM_" .. string.upper( config.plat() ) )
        target:add( "defines", "WV_ARCH_"     .. string.upper( config.arch() ) )
        target:add( "defines", "WV_"          .. string.upper( config.mode() ) )

        if option.get("verbose") then
            print( target:name() )
            print( target:get("defines") )
        end
    end)
end

add_moduledirs( "./" )

function target_platform(_root)
    on_load(function( _target )
        local config = import( "core.project.config" )
        local option = import( "core.base.option" )

        local p = import("get_platform_module")()
        if p.on_load then
            if option.get( "verbose" ) then
                print( "on_load", config.arch() )
            end
            p.on_load( _target )
        end
    end)

    on_run(function( _target )
        local config = import( "core.project.config" )
        local option = import( "core.base.option" )

        local p = import("get_platform_module")()
        if p.on_run then
            if option.get( "verbose" ) then
                print( "on_run", config.arch() )
            end
            p.on_run( _target )
        end
    end)
end


-- 3ds
function init_platform_3ds( target )
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
