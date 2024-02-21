#pragma once

#include <wv/Decl.h>
#include <wv/Math/Vector2.h>

#include <vector>

namespace wv
{
	struct sMouseEvent
	{
		cVector2i position = {0,0};
		
		enum eMouseButton
		{
			MouseButton_None = 0,
			MouseButton_Left,
			MouseButton_Right,
			MouseButton_Middle
		} button = MouseButton_None;
		
		bool button_down = false;
		bool button_up = false;
	};

	class iMouseListener
	{
	WV_DECLARE_INTERFACE( iMouseListener )

	public:

		static void invoke( sMouseEvent _event );

	protected:

		void subscribeMouseEvents  ( void );
		void unsubscribeMouseEvents( void );
		virtual void onMouseEvent( sMouseEvent _event ) = 0;

	private:

		inline static std::vector<iMouseListener*> m_hooks;

	};
}
