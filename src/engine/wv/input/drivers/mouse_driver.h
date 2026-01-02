#pragma once

#include <wv/input/drivers/input_driver.h>
#include <wv/input/input_system.h>

#include <wv/math/vector2.h>

namespace wv {

struct MouseState
{
	wv::Vector2i position;
	wv::Vector2i motion;
	bool buttonStates[ 5 ] = {};
	int scrollDelta = 0;
};

class IMouseDriver : public IInputDriver
{
public:
	IMouseDriver() = default;
	virtual ~IMouseDriver() {
		
	}

protected:
	virtual void initialize( InputSystem* _inputSystem ) override;
	virtual void shutdown( InputSystem* _inputSystem ) override;
	virtual void pollActions( InputSystem* _inputSystem ) = 0;

	void sendTriggerEvents( InputSystem* _inputSystem );
	void sendValueEvents( InputSystem* _inputSystem );
	void sendAxisEvents( InputSystem* _inputSystem );

	bool hasStateChanged( MouseInputs _state ) {
		if( _state >= MOUSE_BUTTON_LEFT && _state <= MOUSE_BUTTON_X2 )
			return m_state.buttonStates[ (size_t)_state ] != m_prevState.buttonStates[ (size_t)_state ];
		
		switch ( _state )
		{
		case MOUSE_SCROLL_DELTA:  return m_state.scrollDelta != m_prevState.scrollDelta;
		case MOUSE_POSITION_AXIS: return m_state.position    != m_prevState.position;
		case MOUSE_MOTION_AXIS:   return m_state.motion      != m_prevState.motion;
		}

		return false;
	}

	MouseState m_state{};
	MouseState m_prevState{};

	uint32_t m_vdID = 0;
};

}