#pragma once

#include <wv/entity/entity.h>
#include <wv/reflection/reflection.h>

struct CrateComponent
{
	wv::Entity* cameraEntity;
	wv::Vector3f positionOffset;

	static inline wv::Reflection reflection{
		wv::reflect( "cameraEntity", &CrateComponent::cameraEntity ),
		wv::reflect( "positionOffset", &CrateComponent::positionOffset )
	};
};