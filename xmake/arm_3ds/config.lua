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
    _target:set( "extension", ".elf" )

    _target:add( "linkdirs", "C:/devkitPro/libctru/lib" )
    _target:add( "links", "citro3d", "citro2d", "ctru", "m" )
end