#pragma once

#include <wv/serialize.h>
#include <wv/math/vector2.h>

namespace wv {

struct OrbitControllerComponent
{
	float orbitDistance = 16.0f;
	wv::Vector2f cameraMove = { 0.0f, 0.0f };

	static inline wv::Reflection reflection{
		wv::reflect( "orbitDistance", &OrbitControllerComponent::orbitDistance ),
		wv::reflect( "cameraMove", &OrbitControllerComponent::cameraMove )
	};
};

}