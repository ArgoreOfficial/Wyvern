#pragma once

#include <wv/Decl.h>
#include <wv/Math/Vector2.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

	///////////////////////////////////////////////////////////////////////////////////////

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

	///////////////////////////////////////////////////////////////////////////////////////

	class iWindowListener
	{
		WV_DECLARE_INTERFACE( iWindowListener )

	public:

		static void invoke( sWindowEvent _event );
		static void setEnabled( bool _enabled );

		///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void subscribeWindowEvents( void );
		void unsubscribeWindowEvents( void );
		virtual void onWindowEvent( sWindowEvent _event ) = 0;

		///////////////////////////////////////////////////////////////////////////////////////

	private:

		static std::vector<iWindowListener*> m_hooks;
		static bool m_enabled;
	};
}

