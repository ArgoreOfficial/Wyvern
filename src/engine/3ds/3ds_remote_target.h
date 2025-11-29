#pragma once

#include <wv/target/iremote_target.h>

namespace wv {

class RemoteTarget3DS : public IRemoteTarget
{
public:
	RemoteTarget3DS( char _address[ 16 ], uint16_t _port ) : IRemoteTarget( _address, _port )
	{
		m_name = "3DS";
	}
	// Inherited via IRemoteTarget
	const std::string getPlatformName() override {
		return "3DS";
	}

	int buildExecutable( const std::string& _mode ) override;
	int remoteLaunchExecutable( const std::string& _name, const std::vector<std::string>& _args ) override;
};

}
