#pragma once

namespace wv {

enum class Scancode
{
	NONE = 0,
	
	// Alphabetical Keys
	A = 4,
	B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,

	// Alphanumerical Keys (1,2,3,4,5,6,7,8,9,0)
	ALPHA_1 = 30,
	ALPHA_2, ALPHA_3, ALPHA_4, ALPHA_5, ALPHA_6, ALPHA_7, ALPHA_8, ALPHA_9, ALPHA_0,

	RETURN = 40,
	ESCAPE,
	BACKSPACE,
	TAB,
	SPACE,
	MINUS,
	EQUALS,
	LEFT_BRACKET,
	RIGHT_BRACKET,
	BACKSLASH,
	// 50 is treated as 49, do not use
	SEMICOLON = 51,
	APOSTROPHE,
	GRAVE,
	COMMA,
	PERIOD,
	SLASH,

	CAPSLOCK = 57,
};

enum class ControllerButton
{
	NONE = 0,
	A, 
	B, 
	X, 
	Y,
	
	DPAD_UP, 
	DPAD_RIGHT, 
	DPAD_DOWN, 
	DPAD_LEFT,

	START, 
	SELECT,
	HOME, // Centre Button

	JOYSTICK_LEFT,
	JOYSTICK_RIGHT,

	SHOULDER_LEFT,
	SHOULDER_RIGHT
};

}