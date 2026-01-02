#pragma once

#include <wv/input/drivers/input_driver.h>
#include <wv/input/input_system.h>

namespace wv {

class IKeyboardDriver : public IInputDriver
{
public:
	virtual ~IKeyboardDriver() {
		
	}

protected:
	virtual void initiailize( InputSystem* _inputSystem ) override;
	virtual void shutdown( InputSystem* _inputSystem ) override;
	virtual void pollActions( InputSystem* _inputSystem ) = 0;

	bool scancodeStateChanged( uint32_t _scancode ) { return m_scancodeStates[ _scancode ] != m_prevScancodeStates[ _scancode ]; }

	void sendTriggerEvents( InputSystem* _inputSystem );
	void sendValueEvents( InputSystem* _inputSystem );
	void sendAxisEvents( InputSystem* _inputSystem );

	bool m_prevScancodeStates[ SCANCODE_MAX ] = { 0 };
	bool m_scancodeStates[ SCANCODE_MAX ] = { 0 };

	uint32_t m_vdID = 0;
};

}