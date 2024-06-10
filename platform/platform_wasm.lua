function load_platform_wasm()
    add_defines("WV_PLATFORM_WASM")
    add_requires( "glm", "libsdl" )

    for i=1, #WASM_PRELOAD_FILES do 
        add_values("wasm.preloadfiles", "game/" .. WASM_PRELOAD_FILES[ i ] .. "@" .. WASM_PRELOAD_FILES[ i ] )
    end
    
    add_ldflags( "-sALLOW_MEMORY_GROWTH" )
    add_ldflags( "-sMAX_WEBGL_VERSION=2" )
    add_ldflags( "-sFULL_ES3=1" )
end

function target_platform_wasm( target )
    -- add supports
    target:add( "deps", "GLAD" )
    import(".platform.support.glm"   )(target)
    import(".platform.support.libsdl")(target)
end