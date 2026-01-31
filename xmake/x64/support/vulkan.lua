function main( target, _prefix )
    target:add( "defines", _prefix .. "_SUPPORT_VULKAN" )
    target:add( "packages", "vulkansdk" )
    target:add( "packages", "vulkan-memory-allocator" )
end