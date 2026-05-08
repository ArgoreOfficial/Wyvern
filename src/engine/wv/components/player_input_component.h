#pragma once

#include <wv/reflection.h>

namespace wv {

struct PlayerInputComponent
{
	int playerIndex = -1;

	static inline wv::Reflection reflection{
		wv::reflect( "playerIndex", &PlayerInputComponent::playerIndex )
	};
};

}