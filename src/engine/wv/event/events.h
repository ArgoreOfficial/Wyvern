#pragma once

#include <wv/event/listener.h>

#include <wv/math/vector2.h>
#include <wv/misc/key.h>

namespace wv
{
	
	struct sWindowEvent
	{
		enum sEventType
		{
			WV_WINDOW_EVENT_NONE,
			WV_WINDOW_FOCUS_LOST,
			WV_WINDOW_FOCUS_GAINED,
			WV_WINDOW_RESIZED,
		};

		sEventType type = WV_WINDOW_EVENT_NONE;

		/// Only applicable when WV_WINDOW_FOCUS_RESIZED
		/// TODO: event data union?
		Vector2i size = { 0, 0 };
	};

	struct sMouseEvent
	{
		enum sMouseButton
		{
			WV_MOUSE_BUTTON_NONE = 0,
			WV_MOUSE_BUTTON_LEFT,
			WV_MOUSE_BUTTON_RIGHT,
			WV_MOUSE_BUTTON_MIDDLE
		};

		enum sMouseButtonAction
		{
			WV_MOUSE_BUTTON_ACTION_DOWN,
			WV_MOUSE_BUTTON_ACTION_UP
		};

		Vector2i position = { 0, 0 };
		Vector2i delta = { 0, 0 };

		sMouseButton button = WV_MOUSE_BUTTON_NONE;

		bool buttondown = false;
		bool buttonup = false;

		sMouseButtonAction action = WV_MOUSE_BUTTON_ACTION_DOWN;

	};

	struct sInputEvent
	{
		eKey key;
		int scancode;
		int mods;

		bool buttondown;
		bool buttonup;
		bool repeat;
	};



	typedef cEventListener<sWindowEvent> cWindowEventListener;
	typedef cEventListener<sMouseEvent> cMouseEventListener;
	typedef cEventListener<sInputEvent> cInputEventListener;

	typedef cEventDispatcher<sWindowEvent> cWindowEventDispatcher;
	typedef cEventDispatcher<sMouseEvent> cMouseEventDispatcher;
	typedef cEventDispatcher<sInputEvent> cInputEventDispatcher;

}