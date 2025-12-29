#pragma once

#include <wv/event/event.h>
#include <wv/math/vector2.h>

namespace wv {

class MouseMoveEvent : public wv::IEvent
{
	WV_REFLECT_TYPE( MouseMoveEvent )
public:
	MouseMoveEvent() = default;
	MouseMoveEvent( const wv::Vector2f& _move, const Vector2i& _position ) :
		move{ _move },
		position{ _position }
	{
	}

	wv::Vector2f move;
	wv::Vector2i position;
};

}