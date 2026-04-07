#pragma once

#include <wv/entity/ecs.h>

namespace wv {

class Entity;

struct CameraComponent;

class ViewVolume;

class CameraManagerSystem : public ISystem
{
	friend class Application;
public:
	virtual void configure( ArchetypeConfig& _config ) override;
	
private:

	void onInternalCameraUpdate();
	void updateCamera( Entity* _entity, CameraComponent& _component );

};

}