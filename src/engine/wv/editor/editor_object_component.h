#pragma once

#include <wv/math/vector3.h>

namespace wv {

struct EditorObjectComponent
{
	bool selected = false;

	Vector3f translateStart{};
};

}