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
	
	const std::vector<Entity*>& getActiveCameras() const { return m_activeCameras; };

private:

	void onInternalCameraUpdate();
	void updateCamera( Entity* _entity, CameraComponent& _component );

	std::vector<Entity*> m_activeCameras{};
};

}