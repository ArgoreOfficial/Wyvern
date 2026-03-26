rule("wv.debug")
    on_config(function(_target)
        if not is_mode("debug") then return end
        
        import("core.project.config")

        _target:set("basename", _target:name() .. "_" .. config.mode() .. "_" .. config.arch())
        
        _target:set( "symbols",  "debug" ) -- enable the debug symbols
        _target:set( "optimize", "none" )  -- disable optimization
        _target:set( "strip",    "none" )  -- disable stripping
        _target:set( "warnings", "extra" ) -- enable extra warnings
        
        -- #5777: '--device-debug (-G)' overrides '--generate-line-info (-lineinfo)' in nvcc
        -- is this needed?
        _target:add("cuflags", "-G")

        _target:add("defines", "WV_DEBUG")
        _target:add("defines", "WV_TRACK_MEMORY") -- enable memory tracking

    end)
rule_end()

rule("wv.release")
    on_config(function(_target)
        if not is_mode("release") then return end

        import("core.project.config")

        _target:set("basename", _target:name() .. "_" .. config.mode() .. "_" .. config.arch())
        
        _target:set("symbols", "hidden")
        _target:set("optimize", "fast")
        _target:set("strip", "debug")
        _target:set("warnings", "extra")

        -- enable NDEBUG macros to disables standard-C assertions
        _target:add("cxflags", "-DNDEBUG")
        _target:add("cuflags", "-DNDEBUG")

        _target:add("defines", "WV_RELEASE")
    end)
rule_end()

rule("wv.package")
    on_config(function(_target)
        if not is_mode("package") then return end
        
        import("core.project.config")

        _target:set("basename", _target:name() .. "_" .. config.arch())

        _target:set("symbols", "none")
        _target:set("optimize", "fastest")
        _target:set("strip", "all")
        _target:set("warnings", "extra")

        -- enable NDEBUG macros to disables standard-C assertions
        _target:add("cxflags", "-DNDEBUG")
        _target:add("cuflags", "-DNDEBUG")
        
        _target:add("defines", "WV_PACKAGE")
    end)
rule_end()