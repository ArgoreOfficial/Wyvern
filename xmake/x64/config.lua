function _add_supports( _target, _prefix )
    _target:add( "files", "./libs/**.c" )
    _target:add( "headerfiles", "./libs/**.h" )
    _target:add( "includedirs", "./libs/glad/include/" )
    
    if is_mode("Package") then
        _target:set("configdir", "package/bin/dat")
    else
        _target:set("configdir", "bin/dat")
    end

    _target:add("configfiles", "dat/*", {onlycopy = true})

    import( "support.libsdl" )( _target, _prefix ) 
    import( "support.imgui" )( _target, _prefix ) 

    _target:add( "defines", { 
        "WV_SUPPORT_OPENGL", 
        "WV_SUPPORT_OPENGLES",
        "WV_ENABLE_SIMD"
    } )
end

function on_load( _target, _prefix )
    _add_supports( _target, _prefix )
     
    if _target:is_arch( "x64" ) then    
        -- icon resource
        _target:add( "files", "$(projectdir)\\resources\\resource.rc" )
        _target:add( "filegroups", "Resources", { rootdir = "$(projectdir)" } ) -- TODO: allow x86 icon resource

    elseif _target:is_arch( "x86" ) and os.arch() == "x64" then
        _target:add( "ldflags", "-static-libgcc -static-libstdc++" )
        
        _target:add( "linkdirs", "C:/msys64/mingw32/bin/" )
        _target:add( "linkdirs", "C:/msys64/mingw32/lib/" )
    end

	_target:set( "targetdir", "./game/Windows" )
	_target:set( "objectdir", "./build/obj/Windows/$(mode)" )
end