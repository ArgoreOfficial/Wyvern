rule("wv.platform.windows")
    on_config(function(_target)
        _target:add( "syslinks", "user32" )
        _target:add( "syslinks", "Xinput", "Xinput9_1_0" )
        
        _target:add( "defines", "WV_PLATFORM_WINDOWS" )

        if is_arch("x64") then
            _target:add( "defines", "WV_X64", "WV_ARCH_X64" )
            _target:add( "defines", "WV_ENABLE_SIMD" )
        end

        -- Language Defines
        _target:set( "languages", "c17", "cxx20" )
        _target:add( "defines", "WV_C17", "WV_CPP20" )
    end)
rule_end()