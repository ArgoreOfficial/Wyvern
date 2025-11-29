
PLATFORMS = {}

add_moduledirs( "./" )

function init_platform(_prefix)

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
                PLATFORMS[ arch_name ].load( _prefix )
            end
            break
        end
    end

    -- configure defines for each platform
    after_load(function (target)
        import( "core.project.config" )
        import( "core.base.option" )

        -- target:add( "defines", _prefix .. "_PLATFORM_" .. string.upper( config.plat() ) )
        target:add( "defines", _prefix .. "_ARCH_" .. string.upper( config.arch() ) )
        target:add( "defines", _prefix .. "_"      .. string.upper( config.mode() ) )

        if option.get("verbose") then
            print( target:name() )
            print( target:get("defines") )
        end
    end)
end

function target_platform( _prefix )
    on_load(function( _target )
        local config = import( "core.project.config" )
        local option = import( "core.base.option" )

        local p = import("get_platform_module")()
        if p.on_load then
            if option.get( "verbose" ) then
                print( "on_load", config.arch() )
            end
            p.on_load( _target, _prefix )
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

    after_build(function( _target )
        local config = import( "core.project.config" )
        local option = import( "core.base.option" )

        local p = import("get_platform_module")()
        if p.after_build then
            if option.get( "verbose" ) then
                print( "after_build", config.arch() )
            end
            p.after_build( _target )
        end
    end)

    after_link(function( _target )
        local config = import( "core.project.config" )
        local option = import( "core.base.option" )

        local p = import("get_platform_module")()
        if p.after_link then
            if option.get( "verbose" ) then
                print( "after_link", config.arch() )
            end
            p.after_link( _target )
        end
    end)
end
