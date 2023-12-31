#pragma once

#include "iEvent.h"

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

	namespace Events
	{

///////////////////////////////////////////////////////////////////////////////////////

		class cKeyEvent : public iEvent<cKeyEvent>
		{

		public:

			cKeyEvent( int _keyCode ) : m_keycode(_keyCode) { }

///////////////////////////////////////////////////////////////////////////////////////

			int getKeyCode( void ) { return m_keycode; }

///////////////////////////////////////////////////////////////////////////////////////

		protected:

			int m_keycode;

		};

///////////////////////////////////////////////////////////////////////////////////////

		class cKeyDownEvent : public iEvent<cKeyDownEvent>
		{

		public:

			cKeyDownEvent( int _keyCode ) : m_keycode( _keyCode ) { }

///////////////////////////////////////////////////////////////////////////////////////

			int getKeyCode( void ) { return m_keycode; }

///////////////////////////////////////////////////////////////////////////////////////

		protected:

			int m_keycode;

		};

///////////////////////////////////////////////////////////////////////////////////////

		class cKeyUpEvent : public iEvent<cKeyUpEvent>
		{

		public:

			cKeyUpEvent( int _keyCode ) : m_keycode( _keyCode ) { }

///////////////////////////////////////////////////////////////////////////////////////

			int getKeyCode( void ) { return m_keycode; }

///////////////////////////////////////////////////////////////////////////////////////

		protected:

			int m_keycode;

		};

///////////////////////////////////////////////////////////////////////////////////////

	}

}
