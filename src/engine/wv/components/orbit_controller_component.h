#pragma once

#include <wv/math/vector2.h>

namespace wv {

struct OrbitControllerComponent
{
	float orbitDistance = 16.0f;
	wv::Vector2f cameraMove = { 0.0f, 0.0f };
};

}