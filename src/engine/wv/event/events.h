#pragma once

#include <wv/event/listener.h>

#include <wv/math/vector2.h>
#include <wv/misc/key.h>

namespace wv
{
	
	struct WindowEvent
	{
		enum EventType
		{
			WV_WINDOW_EVENT_NONE,
			WV_WINDOW_FOCUS_LOST,
			WV_WINDOW_FOCUS_GAINED,
			WV_WINDOW_RESIZED,
		};

		EventType type = WV_WINDOW_EVENT_NONE;

		/// Only applicable when WV_WINDOW_FOCUS_RESIZED
		/// TODO: event data union?
		Vector2i size = { 0, 0 };
	};

	struct MouseEvent
	{
		enum MouseButton
		{
			WV_MOUSE_BUTTON_NONE = 0,
			WV_MOUSE_BUTTON_LEFT,
			WV_MOUSE_BUTTON_RIGHT,
			WV_MOUSE_BUTTON_MIDDLE
		};

		enum MouseButtonAction
		{
			WV_MOUSE_BUTTON_ACTION_DOWN,
			WV_MOUSE_BUTTON_ACTION_UP
		};

		Vector2i position = { 0, 0 };
		Vector2i delta = { 0, 0 };

		MouseButton button = WV_MOUSE_BUTTON_NONE;

		bool buttondown = false;
		bool buttonup = false;

		MouseButtonAction action = WV_MOUSE_BUTTON_ACTION_DOWN;

	};

	struct InputEvent
	{
		Key key;
		int scancode;
		int mods;

		bool buttondown;
		bool buttonup;
		bool repeat;
	};

	typedef EventListener<WindowEvent> WindowEventListener;
	typedef EventListener<MouseEvent> MouseEventListener;
	typedef EventListener<InputEvent> InputEventListener;

	typedef EventDispatcher<WindowEvent> WindowEventDispatcher;
	typedef EventDispatcher<MouseEvent> MouseEventDispatcher;
	typedef EventDispatcher<InputEvent> InputEventDispatcher;

}