
function glfw_supported()
    return is_plat( "windows" ) or 
           is_plat( "macosx" ) or
           is_plat( "linux" )
end
