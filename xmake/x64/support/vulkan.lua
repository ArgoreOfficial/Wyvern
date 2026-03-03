function main( target, _prefix )
    target:add( "defines", _prefix .. "_SUPPORT_VULKAN" )
    target:add( "defines", "VK_NO_PROTOTYPES " )

    target:add( "packages", "vulkan-headers" )
    target:add( "packages", "volk" )
    target:add( "packages", "vulkan-memory-allocator" )
end