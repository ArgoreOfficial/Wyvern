function main( target, _prefix )
    target:add( "defines", _prefix .. "_SUPPORT_VULKAN" )
    target:add( "defines", "VK_USE_PLATFORM_WIN32_KHR" )

    target:add( "packages", "vulkansdk" )
    target:add( "packages", "vulkan-memory-allocator" )
end