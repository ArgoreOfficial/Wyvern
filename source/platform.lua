if is_mode("Debug") then
    set_symbols("debug")
    set_optimize("none")
end

if is_plat("Windows") then
    add_defines( "WV_PLATFORM_WINDOWS" )
end