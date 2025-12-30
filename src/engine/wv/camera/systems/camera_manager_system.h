#pragma once

#include <wv/entity/world_system.h>
#include <wv/entity/entity_component.h>

#include <vector>
#include <unordered_map>

namespace wv {

class CameraComponent;
class ButtonAction;

class CameraManagerSystem : public IWorldSystem
{
	WV_REFLECT_TYPE( CameraManagerSystem )
public:
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

	std::vector<CameraComponent*> m_cameraComponents;
	std::unordered_map<ComponentID, Entity*> m_cameraEntityMap;

	bool m_cameraComponentsChanged = false;

	CameraComponent* m_activeCamera = nullptr;
	float m_orbitDistance = 5.0f;

	ButtonAction* m_jumpAction = nullptr;
	uint32_t m_jumpEventListenerID;
	uint32_t m_jumpReleaseEventListenerID;
};

}