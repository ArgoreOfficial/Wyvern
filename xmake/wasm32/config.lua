function _add_supports( _target, _prefix )
    _target:add( "files", "./libs/**.c" )
    _target:add( "headerfiles", "./libs/**.h" )
    _target:add( "includedirs", "./libs/glad/include/" )
    
    import( "support.libsdl" )( _target, _prefix ) 
    -- import( "support.imgui" )( _target, _prefix ) 

    _target:add( "defines", { 
        "WV_SUPPORT_OPENGL", 
        "WV_SUPPORT_OPENGLES"
    } )
end

function on_load( _target, _prefix )
    _add_supports( _target, _prefix )
    _target:add("ldflags", "-sFULL_ES3");
    _target:add("ldflags", {"--preload-file", "data@/"}, {expand = false, force = true})

    _target:add( "values", "wasm.preloadfiles", "data" )

	_target:set( "targetdir", "./game/Web" )
	_target:set( "objectdir", "./build/obj/Web/$(mode)" )

end