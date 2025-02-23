function _add_supports( _target )
    _target:add( "deps", "GLAD" )

    if _target:is_arch( "x64" ) then
        import( "support.glfw"  )( _target )
    end

    import( "support.assimp" )( _target )
    import( "support.libsdl" )( _target ) 
    import( "support.imgui" )( _target ) 
    import( "support.joltphysics" )( _target ) 

    _target:add( "defines", { 
        "WV_SUPPORT_OPENGL", 
        "WV_SUPPORT_OPENGLES"
    } )
end

function on_load( _target )
    _add_supports( _target )

    if _target:is_arch( "x64" ) then
        -- icon resource
        _target:add( "files", "$(projectdir)\\resources/resource.rc" )
        _target:add( "filegroups", "Resources", { rootdir = "$(projectdir)" } ) -- TODO: allow x86 icon resource

    elseif _target:is_arch( "x86" ) and os.arch() == "x64" then
        _target:add( "ldflags", "-static-libgcc -static-libstdc++" )
        
        _target:add( "linkdirs", "C:/msys64/mingw32/bin/" )
        _target:add( "linkdirs", "C:/msys64/mingw32/lib/" )
    end

end