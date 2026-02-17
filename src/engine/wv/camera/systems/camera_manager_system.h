#pragma once

#include <wv/entity/world_system.h>
#include <wv/entity/entity_component.h>
#include <wv/entity/entity_component_container.h>

#include <wv/math/vector2.h>

#include <vector>
#include <unordered_map>

namespace wv {

class CameraComponent;
class OrbitCameraComponent;
class PlayerInputComponent;

class CameraManagerSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( CameraManagerSystem, IWorldSystem )
public:
	struct EntityData
	{
		Entity* entity;
		PlayerInputComponent* playerInput;
		CameraComponent* camera;
	};

	CameraManagerSystem();
	~CameraManagerSystem();

	bool hasActiveCamera() const { return m_activeCamera != nullptr; }
	void setActiveCamera( CameraComponent* _camera );
	CameraComponent* getActiveCamera() { return m_activeCamera; }

protected:
	virtual void initialize() override;
	virtual void shutdown  () override;

	virtual void registerComponent  ( Entity* _entity, IEntityComponent* _component ) override;
	virtual void unregisterComponent( Entity* _entity, IEntityComponent* _component ) override;

	virtual void update( WorldUpdateContext& _ctx ) override;
	
	std::vector<EntityData>::iterator findEntity( Entity* _entity ) {
		for ( auto it = m_entityDatas.begin(); it != m_entityDatas.end(); it++ )
			if ( it->entity == _entity ) return it;
		return m_entityDatas.end();
	}

	EntityComponentContainer<OrbitCameraComponent> m_cameraComponents;
	EntityComponentContainer<PlayerInputComponent> m_playerInputComponents;

	std::vector<EntityData> m_entityDatas;

	bool m_cameraComponentsChanged = false;

	CameraComponent* m_activeCamera = nullptr;

	uint32_t m_lookAction = 0;

	float m_orbitDistance = 16.0f; 
	wv::Vector2f m_cameraMove = { 0.0f, 0.0f };

private:
	WV_REFLECT_MEMBER( m_orbitDistance, "Orbit Distance" )
	WV_REFLECT_MEMBER( m_cameraMove, "Camera Move" )
};

}