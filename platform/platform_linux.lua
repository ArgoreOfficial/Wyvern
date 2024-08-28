function load_platform_linux()
    add_requires( "glfw", "libsdl", "assimp" )
end

function target_platform_linux( target )
    local root = "../"
    
    -- add supports
    target:add( "deps", "GLAD" )
    import(root.."platform.support.glfw"  )(target)
    import(root.."platform.support.assimp")(target)
    import(root.."platform.support.libsdl")(target) 
end