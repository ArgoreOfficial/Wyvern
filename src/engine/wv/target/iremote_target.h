#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <cstring>

namespace wv {

class IRemoteTarget
{
public:
	IRemoteTarget( char _addr[ 16 ], uint16_t _port )
	{
		setAddress( _addr, _port );
	}
	
	void setAddress( char _addr[ 16 ], uint16_t _port ) {
		std::memcpy( m_address, _addr, 16 );
		m_port = _port;
	}

	virtual std::string getName() {
		return m_name;
	}
	
	virtual const std::string getPlatformName() = 0;
	virtual int remoteLaunchExecutable( const std::string& _name, const std::vector<std::string>& _args ) = 0;

protected:
	std::string m_name;

	char     m_address[ 16 ] = "192.168.0.1";
	uint16_t m_port          = 25565;

};

}
