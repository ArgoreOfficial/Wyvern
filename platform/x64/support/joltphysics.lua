function main(target)
    target:add( "defines", "WV_SUPPORT_JOLT_PHYSICS" )
    target:add( "defines", "WV_SUPPORT_PHYSICS" )
    
    target:add( "defines", "JPH_OBJECT_STREAM" )
    
    target:add( "packages", "joltphysics" )
end