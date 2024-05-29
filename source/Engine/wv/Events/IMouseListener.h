#pragma once

#include <wv/Decl.h>
#include <wv/Math/Vector2.h>

#include <vector>

namespace wv
{
	struct MouseEvent
	{
		Vector2i position = {0,0};
		
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

	class IMouseListener
	{
	WV_DECLARE_INTERFACE( IMouseListener )

	public:

		static void invoke( MouseEvent _event );

	protected:

		void subscribeMouseEvents  ( void );
		void unsubscribeMouseEvents( void );
		virtual void onMouseEvent( MouseEvent _event ) = 0;

	private:

		inline static std::vector<IMouseListener*> m_hooks;

	};
}
