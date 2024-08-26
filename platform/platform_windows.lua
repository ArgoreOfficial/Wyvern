
function load_platform_windows()
    set_languages( "c17", "cxx20" )
    add_defines( "WV_C17", "WV_CPP20" )

    add_requires( "glm", "libsdl", "assimp", "joltphysics" )
    add_requires( "imgui v1.91.0-docking", { 
        configs={
            opengl3 = true,
            glfw    = true,
            sdl2    = true
        }})

    if( is_arch( "x64" ) ) then
        add_requires( "glfw" )
    end

    add_defines( "WV_SUPPORT_OPENGL", "WV_SUPPORT_OPENGLES" )
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
    import(root.."platform.support.joltphysics")(target) 
end