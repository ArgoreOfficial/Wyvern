#pragma once

#include <wv/math/vector3.h>
#include <wv/math/rotor.h>

namespace wv {

struct EditorObjectComponent
{
	bool selected = false;

	Vector3f translateStart{};
	Rotorf rotateStart{};
	Vector3f scaleStart{};

};

}