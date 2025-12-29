#pragma once

#include <wv/debug/log.h>
#include <wv/debug/error.h>

#include <wv/math/vector2.h>
#include <wv/input/action_group.h>
#include <wv/input/input_enums.h>

#include <vector>

namespace wv {

class InputSystem
{
public:
	InputSystem() = default;
	~InputSystem();

	void updateInputDrivers();
	void processInputEvents();

	ActionGroup* getActionGroup( const std::string& _name ) const { 
		if ( !m_actionGroupNameMap.contains( _name ) )
		{
			WV_LOG_ERROR( "There is no action group named '%s'\n", _name.c_str() );
			return nullptr;
		}
		return m_actionGroupNameMap.at( _name ); 
	}
	ActionGroup* createActionGroup( const std::string& _name );
	void destroyActionGroup( const std::string& _name );
	
#ifndef WV_PACKAGE
	inline wv::Vector2f debugGetMouseMotion()   const { return m_debugMouseMotion; }
	inline wv::Vector2f debugGetMousePosition() const { return m_debugMousePosition; }
	
	inline bool debugIsMouseDown( int _index ) {
		WV_ASSERT( _index < 0 );
		WV_ASSERT( _index >= 5 );
		return m_debugMouseButtonStates[ _index ];
	}
#endif

protected:
	enum class DriverInputEventType
	{
		UNUSUED = 0,

		GAMEPAD_JOYSTICK_UPDATE,
		GAMEPAD_BUTTON_DOWN,
		GAMEPAD_BUTTON_UP,

		KEY_DOWN,
		KEY_UP,

		MOUSE_DOWN,
		MOUSE_UP,
		MOUSE_MOVE,
		MOUSE_WHEEL
	};

	struct DriverInputEvent
	{
		DriverInputEventType eventType = DriverInputEventType::UNUSUED;

		// Mouse

		wv::Vector2f mouseMotion{ 0.0f, 0.0f }; // delta mouse motion
		wv::Vector2f mousePosition{ 0.0f, 0.0f }; // absolute mouse position

		int mouseButtonID = 0; // mouse button index

		// Keyboard

		wv::Scancode scancode = Scancode::NONE;
		bool isRepeat = false;

		// Controller

		wv::ControllerButton controllerButton = wv::ControllerButton::NONE;
	};

#ifndef WV_PACKAGE
	wv::Vector2f m_debugMouseMotion{ 0.0f, 0.0f };
	wv::Vector2f m_debugMousePosition{ 0.0f, 0.0f };
	bool m_debugMouseButtonStates[ 5 ] = { false, false, false, false, false };
#endif

	std::vector<DriverInputEvent> m_driverEvents;

	std::vector<ActionGroup*> m_actionGroups;
	std::unordered_map<std::string, ActionGroup*> m_actionGroupNameMap;
};

}