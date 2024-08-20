
function load_platform_windows()
    add_requires( "glm", "libsdl", "assimp", "imgui" )
    
    if( is_arch( "x64" ) ) then
        add_requires( "glfw" )
    end
end

function target_platform_windows( target )
    local root = "../"
    -- add supports
    target:add( "deps", "GLAD" )
    import(root.."platform.support.glm")(target)
    
    if target:is_arch( "x64" ) then
        import(root.."platform.support.glfw"  )(target)

        -- icon resource
        target:add( "files", "$(projectdir)\\resources/resource.rc" )
        target:add( "filegroups", "Resources", { rootdir = "$(projectdir)" } ) -- TODO: allow x86 icon resource

    elseif target:is_arch( "x86" ) and os.arch() == "x64" then
        target:add( "ldflags", "-static-libgcc -static-libstdc++" )
        
        target:add( "linkdirs", "C:/msys64/mingw32/bin/" )
        target:add( "linkdirs", "C:/msys64/mingw32/lib/" )

    end

    import(root.."platform.support.assimp")(target)
    import(root.."platform.support.libsdl")(target) 
    import(root.."platform.support.imgui")(target) 
end