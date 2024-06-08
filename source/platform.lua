
if is_mode("Debug") then
    set_symbols "debug"
    set_optimize "none"
elseif is_mode("Release") then
    set_symbols "debug"
    set_optimize "faster"
elseif is_mode("Package") then
    set_symbols "none"
    set_optimize "fastest"
end


-- configuration defines

if is_mode("Debug")   then add_defines("WV_DEBUG") end
if is_mode("Release") then add_defines("WV_RELEASE") end
if is_mode("Package") then add_defines("WV_PACKAGE") end


-- platform defines 

if is_plat("windows") then add_defines("WV_PLATFORM_WINDOWS") end
if is_plat("wasm") then add_defines("WV_PLATFORM_WINDOWS") end

-- dependencies

add_requires( "glm" )

if not is_plat( "wasm" ) then
    add_requires( "glfw" )
end