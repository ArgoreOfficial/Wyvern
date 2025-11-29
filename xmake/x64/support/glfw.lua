function main( target, _prefix )
    target:add( "defines", _prefix .. "_SUPPORT_GLFW" )
    target:add( "packages", "glfw" )
end