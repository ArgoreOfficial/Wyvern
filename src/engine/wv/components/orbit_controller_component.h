#pragma once

#include <wv/serialize.h>
#include <wv/math/vector2.h>

namespace wv {

struct OrbitControllerComponent
{
	float orbitDistance = 16.0f;
	wv::Vector2f cameraMove = { 0.0f, 0.0f };
};

template<>
static void serialize<OrbitControllerComponent>( SerializeInfo& _info )
{
	_info.name = "OrbitControllerComponent";
	_info.registerMember( &OrbitControllerComponent::orbitDistance, "orbitDistance" );
	_info.registerMember( &OrbitControllerComponent::cameraMove, "cameraMove" );
}

}