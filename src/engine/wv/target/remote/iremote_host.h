#pragma once

#include <string>

namespace wv {

class IRemoteClient;

class IRemoteHost
{
public:

	virtual void launchHost( const std::string& _targetIP, uint16_t _port ) = 0;
	virtual void connectClient( IRemoteClient* ) = 0;

};

}
