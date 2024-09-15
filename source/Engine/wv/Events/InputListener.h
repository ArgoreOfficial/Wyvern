#pragma once

#include <wv/Decl.h>

#include <vector>

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct InputEvent
	{
		int key;
		int scancode;
		int mods;

		bool buttondown;
		bool buttonup;
		bool repeat;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class iInputListener
	{
	WV_DECLARE_INTERFACE( iInputListener )

	public:

		static void invoke( InputEvent _event );
		static void setEnabled( bool _enabled ) { m_enabled = _enabled; }

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void subscribeInputEvent  ( void );
		void unsubscribeInputEvent( void );
		virtual void onInputEvent( InputEvent _event ) = 0;

///////////////////////////////////////////////////////////////////////////////////////

	private:

		static std::vector<iInputListener*> m_hooks;
		static bool m_enabled;

	};

}

