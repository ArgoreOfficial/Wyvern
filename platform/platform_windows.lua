function load_platform_windows()
    add_defines("WV_PLATFORM_WINDOWS")
    add_requires( "glm", "glfw", "libsdl" )
end

function target_platform_windows( target )
    local root = "../"
    
    -- icon resource
    target:add( "files", "$(projectdir)\\resources/resource.rc" )
    target:add( "filegroups", "Resources", { rootdir = "$(projectdir)" } )
    
    -- add supports
    target:add( "deps", "GLAD" )
    import(root.."platform.support.glm"   )(target)
    import(root.."platform.support.glfw"  )(target)
    import(root.."platform.support.assimp")(target)
    import(root.."platform.support.libsdl")(target) 
end