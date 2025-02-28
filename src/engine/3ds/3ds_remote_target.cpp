#include "3ds_remote_target.h"

#include <wv/debug/log.h>

int wv::RemoteTarget3DS::remoteLaunchExecutable( const std::string& _name, const std::vector<std::string>& _args )
{
	int pingErr = wv::Console::run( {
		"ping",
		"-n 1",
		m_address,
		"> nul"
	} );

	if( pingErr )
	{
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Failed to ping 3DS '%s'\n", m_address );
		return pingErr;
	}

	int err = wv::Console::run( { 
		"../../tools/3ds/3dslink",
		"-a", m_address,
		"-0 \"RMT\"", // argv[0] == "RMT" -> launches remote client
		"../3ds/" + _name + ".3dsx" 
	} );

	return err;
}
