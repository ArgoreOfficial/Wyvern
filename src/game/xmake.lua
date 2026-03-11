target("Game")
    set_kind "binary"
    add_deps("Wyvern")

    set_warnings( "extra" )

    if is_mode("Package") then
        set_basename("Game_$(arch)")
    else
        set_basename("Game_$(mode)_$(arch)")
    end
    
    add_headerfiles("**.h", {install = false})
    add_headerfiles("**.hpp", {install = false})
    add_files("**.cpp")
    add_includedirs(
        "./",
        "../engine/"
    )
target_end()

