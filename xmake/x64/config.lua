function on_load( target )
    -- add supports
    --target:add( "deps", "GLAD" )

    if target:is_arch( "x64" ) then
        --import( "support.glfw"  )(target)

        -- icon resource
        target:add( "files", "$(projectdir)\\resources/resource.rc" )
        target:add( "filegroups", "Resources", { rootdir = "$(projectdir)" } ) -- TODO: allow x86 icon resource

    elseif target:is_arch( "x86" ) and os.arch() == "x64" then
        target:add( "ldflags", "-static-libgcc -static-libstdc++" )
        
        target:add( "linkdirs", "C:/msys64/mingw32/bin/" )
        target:add( "linkdirs", "C:/msys64/mingw32/lib/" )
    end

    --import( "support.assimp" )( target )
    --import( "support.libsdl" )( target ) 
    --import( "support.imgui" )( target ) 
    --import( "support.joltphysics" )( target ) 
end