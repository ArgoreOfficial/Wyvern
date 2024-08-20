function main(target)
    target:add( "defines", "WV_JOLT_PHYSICS_SUPPORTED" )
    target:add( "defines", "JPH_OBJECT_STREAM" )
    
    target:add( "packages", "joltphysics" )
end