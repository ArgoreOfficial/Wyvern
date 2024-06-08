
function glfw_supported()
    return is_plat( "windows" ) or 
           is_plat( "macosx" ) or
           is_plat( "linux" )
end



if is_mode("Debug") then
end

set_symbols "debug"
set_optimize "none"


-- configuration defines

if is_mode("Debug")   then add_defines("WV_DEBUG") end
if is_mode("Release") then add_defines("WV_RELEASE") end
if is_mode("Package") then add_defines("WV_PACKAGE") end


-- platform defines 

if is_plat("windows") then add_defines("WV_PLATFORM_WINDOWS") end
if is_plat("wasm") then add_defines("WV_PLATFORM_WINDOWS") end

-- dependencies

add_requires( "glm" )
add_requires( "libsdl" )

if glfw_supported() then 
    add_requires( "glfw" )
    add_defines( "WV_GLFW_SUPPORTED" )
end
