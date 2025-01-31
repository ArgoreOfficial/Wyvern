function on_load( _target )
    local DEVKITPRO = import( "modules.find_devkitpro" ).find_devkitpro()
    local DEVKITARM = path.join( DEVKITPRO, "devkitARM" )
    local BINDIR = path.join( DEVKITARM, "bin" )
    
    _target:add( "includedirs", {
        path.join( DEVKITPRO, "/libctru/include" ), 
        --path.join( DEVKITARM, "/include" ), 
        path.join( DEVKITARM, "/arm-none-eabi/include" ), 
        path.join( DEVKITARM, "/arm-none-eabi/include/c++/14.2.0" ), 
        --path.join( DEVKITARM, "/arm-none-eabi/include/c++/14.2.0/arm-none-eabi" ), 
        --path.join( DEVKITARM, "/lib/gcc/arm-none-eabi/14.2.0/include" )
    } )

    --_target:add( "linkdirs", path.join( DEVKITPRO, "/libctru/lib" ) )
    --_target:add( "syslinks", "citro3d", "citro2d", "ctru", "m" )

    _target:add( "files", "source/**.pica", {rule = "3ds.picasso"} )
    _target:add( "rules", "3ds.package" )

    local kind = _target:get("kind")
    if kind == "binary" then
        _target:set( "extension", ".elf" )
    elseif kind == "static" then
        _target:set( "extension", ".a" )
    end

    _target:add( "linkdirs", "C:/devkitPro/libctru/lib" )
    _target:add( "links", "citro3d", "citro2d", "ctru", "m" )
end

function after_link(_target)
    if _target:get( "kind" ) ~= "binary" then
        return
    end

    local DEVKITPRO = import( "modules.find_devkitpro" ).find_devkitpro()
    local DEVKITARM = path.join( DEVKITPRO, "devkitARM" )

    local namepath = _target:targetdir() .. "/" .. _target:basename()

    os.vrunv( path.join( DEVKITARM, "bin/arm-none-eabi-gcc-nm" ), { 
        "-CSn", namepath .. ".elf"
        }, {stdout = namepath .. ".lst"} )

    os.vrunv( path.join( DEVKITPRO, "tools/bin/3dsxtool" ), { 
        namepath .. ".elf",
        namepath .. ".3dsx",
        -- "--smdh=/d/dev/3ds/3ds.smdh"
        })
end