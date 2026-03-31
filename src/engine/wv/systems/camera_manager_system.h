#pragma once

#include <wv/entity/ecs.h>

namespace wv {

class Entity;

struct CameraComponent;

class ViewVolume;

class CameraManagerSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override;
	
	virtual void onUpdate() override;

private:
	void updateCamera( Entity* _entity, CameraComponent& _component );

};

}