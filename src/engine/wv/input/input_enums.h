#pragma once

#include <stdint.h>

namespace wv {

enum Scancode : uint32_t
{
	Scancode_None = 0,
	
	// Alphabetical Keys
	Scancode_A,
	Scancode_B,
	Scancode_C,
	Scancode_D,
	Scancode_E,
	Scancode_F,
	Scancode_G,
	Scancode_H,
	Scancode_I,
	Scancode_J,
	Scancode_K,
	Scancode_L,
	Scancode_M,
	Scancode_N,
	Scancode_O,
	Scancode_P,
	Scancode_Q,
	Scancode_R,
	Scancode_S,
	Scancode_T,
	Scancode_U,
	Scancode_V,
	Scancode_W,
	Scancode_X,
	Scancode_Y,
	Scancode_Z,

	// Alphanumerical Keys (1,2,3,4,5,6,7,8,9,0)
	Scancode_1,
	Scancode_2, 
	Scancode_3, 
	Scancode_4, 
	Scancode_5, 
	Scancode_6, 
	Scancode_7, 
	Scancode_8, 
	Scancode_9, 
	Scancode_0,

	Scancode_Return,
	Scancode_Escape,
	Scancode_Backspace,
	Scancode_Tab,
	Scancode_Space,
	Scancode_Mins,
	Scancode_Equals,
	Scancode_LeftBracket,
	Scancode_RightBracket,
	Scancode_Backslash,
	Scancode_SemiColon,
	Scancode_Apostrophe,
	Scancode_Grave,
	Scancode_Comma,
	Scancode_Period,
	Scancode_Slash,
	Scancode_Capslock,

	Scancode_ArrowUp,
	Scancode_ArrowDown,
	Scancode_ArrowLeft,
	Scancode_ArrowRight,

	Scancode_LeftShift,
	Scancode_RightShift,

	Scancode_MAX
};

enum MouseInputs : uint32_t
{
	// Only Trigger and Value actions

	MouseInput_Left = 0,
	MouseInput_Middle,
	MouseInput_Right,
	MouseInput_X1,
	MouseInput_X2,

	MouseInput_ScrollDelta,

	// Only axis actions

	MouseInput_PositionAxis,
	MouseInput_MotionAxis
};

enum ControllerInputs : uint32_t
{
	ControllerInput_None = 0,
	ControllerInput_ButtonA = 1 << 0, 
	ControllerInput_ButtonB = 1 << 1, 
	ControllerInput_ButtonX = 1 << 2, 
	ControllerInput_ButtonY = 1 << 3,
	
	ControllerInput_DpadUp    = 1 << 4,
	ControllerInput_DpadRight = 1 << 5,
	ControllerInput_DpadDown  = 1 << 6,
	ControllerInput_DpadLeft  = 1 << 7,

	ControllerInput_Start  = 1 << 8,
	ControllerInput_Select = 1 << 9,
	ControllerInput_Home   = 1 << 10, // Centre Button

	ControllerInput_JoystickButtonLeft  = 1 << 11,
	ControllerInput_JoystickButtonRight = 1 << 12,

	ControllerInput_ShoulderLeft  = 1 << 13,
	ControllerInput_ShoulderRight = 1 << 14,

	// Joystick

	ControllerInput_JoystickLeft  = 1 << 15,
	ControllerInput_JoystickRight = 1 << 16,

	// Analogue Triggers

	ControllerInput_TriggerLeft  = 1 << 17,
	ControllerInput_TriggerRight = 1 << 18,

	ControllerInput_MAX
};

}