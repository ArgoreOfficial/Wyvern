function main( target, _prefix )
    target:add( "defines", _prefix .. "_SUPPORT_TRACY" )
    --target:add( "defines", "TRACY_ENABLE" ) -- use this to enable profiling
    
    target:add( "packages", "tracy" )
end