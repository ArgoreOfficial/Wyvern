#pragma once

#include <wv/Decl.h>
#include <wv/Math/Vector2.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	struct MouseEvent
	{
		Vector2i position = { 0, 0 };
		Vector2i delta = { 0, 0 };

		enum MouseButton
		{
			WV_MOUSE_BUTTON_NONE = 0,
			WV_MOUSE_BUTTON_LEFT,
			WV_MOUSE_BUTTON_RIGHT,
			WV_MOUSE_BUTTON_MIDDLE
		} button = WV_MOUSE_BUTTON_NONE;
		
		bool buttondown = false;
		bool buttonup = false;

		enum MouseButtonAction
		{
			WV_MOUSE_BUTTON_ACTION_DOWN,
			WV_MOUSE_BUTTON_ACTION_UP
		} action;

	};

///////////////////////////////////////////////////////////////////////////////////////

	class iMouseListener
	{
	WV_DECLARE_INTERFACE( iMouseListener )

	public:

		static void invoke( MouseEvent _event );
		static void setEnabled( bool _enabled ) { m_enabled = _enabled; }

///////////////////////////////////////////////////////////////////////////////////////

	protected:

		void subscribeMouseEvents  ( void );
		void unsubscribeMouseEvents( void );
		virtual void onMouseEvent( MouseEvent _event ) = 0;

///////////////////////////////////////////////////////////////////////////////////////

	private:

		static std::vector<iMouseListener*> m_hooks;
		static bool m_enabled;

	};

}
