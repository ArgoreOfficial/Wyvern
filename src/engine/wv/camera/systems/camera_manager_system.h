#pragma once

#include <wv/entity/world_system.h>
#include <wv/entity/entity_component.h>

#include <wv/math/vector2.h>

#include <vector>
#include <unordered_map>

namespace wv {

class CameraComponent;
class PlayerInputComponent;

class CameraManagerSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( CameraManagerSystem )
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

	void update( WorldUpdateContext& _ctx ) override;

	std::vector<EntityData>::iterator findEntity( Entity* _entity ) {
		for ( auto it = m_entityDatas.begin(); it != m_entityDatas.end(); it++ )
			if ( it->entity == _entity ) return it;
		return m_entityDatas.end();
	}

	std::vector<CameraComponent*> m_cameraComponents;
	std::vector<PlayerInputComponent*> m_playerInputComponents;

	std::unordered_map<ComponentID, Entity*> m_componentEntityMap;
	std::vector<EntityData> m_entityDatas;

	bool m_cameraComponentsChanged = false;

	CameraComponent* m_activeCamera = nullptr;
	float m_orbitDistance = 25.0f;

	uint32_t m_lookAction = 0;

	uint32_t m_jumpEventListenerID = 0;
	uint32_t m_jumpReleaseEventListenerID = 0;

	wv::Vector2f m_cameraMove = { 0.0f, 0.0f };

};

}