#pragma once
#include "Event.h"

namespace WV
{
	class KeyEvent : public Event
	{
		inline int getKeyCode() { return m_keycode; }
	protected:
		KeyEvent(int _keycode):
			m_keycode(_keycode) { }

		int m_keycode;
	};

	class KeyDownEvent : public KeyEvent
	{
	public:
		KeyDownEvent();
	};

	class KeyUpEvent : public KeyEvent
	{
	public:
		KeyUpEvent();
	};
}
