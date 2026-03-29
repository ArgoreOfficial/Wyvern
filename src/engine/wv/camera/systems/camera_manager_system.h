#pragma once

#include <wv/entity/ecs.h>
#include <wv/math/vector2.h>

namespace wv {

struct CameraComponent;
struct OrbitCameraComponent;
struct PlayerInputComponent;

class ViewVolume;

class CameraManagerSystem : public ISystem
{
//	WV_REFLECT_TYPE( CameraManagerSystem, ISystem )
public:
	CameraManagerSystem();
	~CameraManagerSystem();

	bool hasActiveCamera() const { return m_activeCamera != nullptr; }
	void setActiveCamera( CameraComponent* _camera );
	CameraComponent* getActiveCamera() { return m_activeCamera; }

protected:
	virtual void configure( ArchetypeConfig& _config ) override;
	virtual void initialize() override;
	virtual void shutdown  () override;

	virtual void update() override;
	
	CameraComponent* m_activeCamera = nullptr;

	float m_orbitDistance = 16.0f; 
	wv::Vector2f m_cameraMove = { 0.0f, 0.0f };

private:
//	WV_REFLECT_MEMBER( m_orbitDistance, "Orbit Distance" )
//	WV_REFLECT_MEMBER( m_cameraMove, "Camera Move" )
};

}