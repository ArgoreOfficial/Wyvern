function find_devkitpro()
    local devkitpro = os.getenv( "DEVKITPRO" )
    if devkitpro then
        devkitpro = devkitpro:gsub("%/opt/", "C:/")
        return devkitpro
    end
end