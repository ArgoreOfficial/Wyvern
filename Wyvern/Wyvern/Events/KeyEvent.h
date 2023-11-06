#pragma once
#include "IEvent.h"

namespace WV
{
	namespace Events
	{
		class KeyEvent : public IEvent<KeyEvent>
		{
		public:
			KeyEvent( int _keyCode ) : m_keycode(_keyCode) { }
			inline int getKeyCode() { return m_keycode; }

		protected:
			int m_keycode;
		};

		class KeyDownEvent : public IEvent<KeyDownEvent>
		{
		public:
			KeyDownEvent( int _keyCode ) : m_keycode( _keyCode ) { }
			inline int getKeyCode() { return m_keycode; }

		protected:
			int m_keycode;
		};

		class KeyUpEvent : public IEvent<KeyUpEvent>
		{
		public:
			KeyUpEvent( int _keyCode ) : m_keycode( _keyCode ) { }
			inline int getKeyCode() { return m_keycode; }

		protected:
			int m_keycode;
		};
	}
}
