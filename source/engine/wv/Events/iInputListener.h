#pragma once

#include <wv/Decl.h>

#include <vector>

namespace wv
{
	struct sInputEvent
	{
		int key;
		int scancode;
		int mods;

		bool buttondown;
		bool buttonup;
		bool repeat;
	};

	class iInputListener
	{
	WV_DECLARE_INTERFACE( iInputListener )

	public:
		static void invoke( sInputEvent _info );

	protected:

		inline  void subscribeInputEvent  ( void );
		inline  void unsubscribeInputEvent( void );
		virtual void onInputEvent( sInputEvent _info ) = 0;

	private:

		inline static std::vector<iInputListener*> m_hooks;

	};
}

