#pragma once

#include <wv/math/vector2.h>
#include <wv/math/matrix.h>

namespace wv {

struct Viewport
{
	inline double getAspect() const { return (double)size.x / (double)size.y; }

	wv::Matrix4x4f viewProj{};
	wv::Vector2i size{ 900, 600 };
	
};

}