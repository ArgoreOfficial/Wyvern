#pragma once

#include <wv/input/drivers/input_driver.h>
#include <wv/memory/memory.h>

#include <wv/math/vector2.h>

#include <vector>
#include <set>

namespace wv {

struct ControllerDevice
{
	int deviceID;

	wv::Vector2f leftJoystick;
	wv::Vector2f rightJoystick;
};

class IControllerDriver : public IInputDriver
{
public:
	virtual ~IControllerDriver() {
		for ( ControllerDevice* device : m_connectedDevices )
			WV_FREE( device );
	}

protected:
	std::vector<ControllerDevice*>::iterator getDevice( int _deviceID ) {
		for ( auto it = m_connectedDevices.begin(); it != m_connectedDevices.end(); )
		{
			if ( ( *it )->deviceID != _deviceID )
				continue;
			return it;
		}

		return m_connectedDevices.end();
	}

	virtual void updateDriver( InputSystem* _inputSystem ) = 0;

	std::set<int> m_connectedDeviceIDs;
	std::vector<ControllerDevice*> m_connectedDevices;
};

}