#pragma once

#include <wv/Decl.h>

#include <vector>

namespace wv
{
	struct InputEvent
	{
		int key;
		int scancode;
		int mods;

		bool buttondown;
		bool buttonup;
		bool repeat;
	};

	class IInputListener
	{
	WV_DECLARE_INTERFACE( IInputListener )

	public:
		static void invoke( InputEvent _event );

	protected:

		void subscribeInputEvent  ( void );
		void unsubscribeInputEvent( void );
		virtual void onInputEvent( InputEvent _event ) = 0;

	private:

		inline static std::vector<IInputListener*> m_hooks;

	};
}

