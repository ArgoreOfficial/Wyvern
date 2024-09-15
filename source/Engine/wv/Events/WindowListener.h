#pragma once

#include <wv/Decl.h>
#include <wv/Math/Vector2.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

	///////////////////////////////////////////////////////////////////////////////////////

	struct WindowEvent
	{
		enum EventType
		{
			WV_WINDOW_FOCUS_LOST,
			WV_WINDOW_FOCUS_GAINED,
			WV_WINDOW_RESIZED,
		} type;

		/// Only applicable when WV_WINDOW_FOCUS_RESIZED
		Vector2i size = { 0, 0 };
	};

	///////////////////////////////////////////////////////////////////////////////////////

	class iWindowListener
	{
		WV_DECLARE_INTERFACE( iWindowListener )

	public:

		static void invoke( WindowEvent _event );
		static void setEnabled( bool _enabled );

		///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void subscribeWindowEvents( void );
		void unsubscribeWindowEvents( void );
		virtual void onWindowEvent( WindowEvent _event ) = 0;

		///////////////////////////////////////////////////////////////////////////////////////

	private:

		static std::vector<iWindowListener*> m_hooks;
		static bool m_enabled;
	};
}

