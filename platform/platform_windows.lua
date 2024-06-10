function load_platform_windows()
    add_defines("WV_PLATFORM_WINDOWS")
    add_requires( "glm", "glfw", "libsdl" )
end

function target_platform_windows( target )
    local root = "$(projectdir)\\"

    -- icon resource
    target:add( "files", root .. "resources/resource.rc" )
    target:add( "filegroups", "Resources", { rootdir = root .. "resources" } )
    
    -- add supports
    target:add( "deps", "GLAD" )
    import(".platform.support.glm"   )(target)
    import(".platform.support.glfw"  )(target)
    import(".platform.support.assimp")(target)
    import(".platform.support.libsdl")(target)
end