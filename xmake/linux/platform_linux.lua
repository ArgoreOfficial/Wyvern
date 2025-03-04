function load_platform_linux()
    add_requires( "libsdl", "assimp" )
end

function target_platform_linux( target )
    local root = "../"
    
    -- add supports
    target:add( "deps", "GLAD" )
    import(root.."platform.support.assimp")(target)
    import(root.."platform.support.libsdl")(target) 
end

table.insert( PLATFORMS, { 
    plat="linux", 
    arch={ "x86_64" }, 
    load=load_platform_linux, 
    target=target_platform_linux 
} )
