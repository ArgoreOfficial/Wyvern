function main()
	local config = import( "core.project.config" )
    local module_name = path.join( config.arch(), "config" )

    return import( module_name )
end