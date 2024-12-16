
PLATFORMS = {
    -- { plat="3ds",     arch={ "3ds-arm"     }, load=load_platform_3ds,     target=target_platform_3ds     },
    -- { plat="psvita",  arch={ "psvita"      }, load=load_platform_psvita,  target=target_platform_psvita, run=run_platform_psvita  }
}

function load_platform()

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

    local rpath = "./platform/platforms/"
    for _, file in ipairs(os.files(rpath .. "*.lua"), path.basename) do
        includes( file )
    end

    for i=1,#PLATFORMS do 
        if is_arch(table.unpack(PLATFORMS[i].arch)) and PLATFORMS[ i ].load ~= nil then
            PLATFORMS[i].load()
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


function target_platform(_root)
    for i=1,#PLATFORMS do 
        if is_arch(table.unpack(PLATFORMS[i].arch)) then 
            if PLATFORMS[ i ].load ~= nil then
                on_load(PLATFORMS[i].target)
            end
            if PLATFORMS[ i ].run ~= nil then
                on_run( PLATFORMS[ i ].run )
            end
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
