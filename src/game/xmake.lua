target(PROJECT_NAME)
    set_kind "binary"
    add_deps "Wyvern"

    set_warnings( "extra" )

    if is_mode("Package") then
        set_basename(PROJECT_NAME .. "_$(arch)")
    else
        set_basename(PROJECT_NAME .. "_$(mode)_$(arch)")
    end
    
    add_headerfiles("**.h", "**.hpp")
    add_files("**.cpp")
    add_includedirs(
        "./",
        "../engine/"
    )
    
    target_platform(ENGINE_NAMESPACE)
target_end()

