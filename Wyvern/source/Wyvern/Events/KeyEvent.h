#pragma once

namespace WV
{
	class KeyEvent
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
