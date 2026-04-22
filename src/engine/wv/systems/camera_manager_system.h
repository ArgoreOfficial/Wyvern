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

	Entity* getCameraOverride()            const { return m_cameraOverride; }
	void    setCameraOverride( Entity* _camera ) { m_cameraOverride = _camera; }

private:

	void onInternalCameraUpdate();
	void updateCamera( Entity* _entity, CameraComponent& _component );

	std::vector<Entity*> m_activeCameras{};
	Entity* m_cameraOverride = nullptr;
};

}