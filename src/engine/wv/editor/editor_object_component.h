#pragma once

#include <wv/serialize.h>

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

template<>
static void serialize<EditorObjectComponent>( SerializeInfo& _info )
{
	_info.name = "EditorObjectComponent";
	_info.registerMember( &EditorObjectComponent::selected, "selected" );
}

}