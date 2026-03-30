#pragma once

#include <wv/entity/ecs.h>
#include <wv/input/input_system.h>
#include <wv/reflection/reflection.h>

namespace wv {

class Entity;
class InputSystem;

class MeshManager;
class MaterialManager;
class TextureManager;

struct Viewport;

struct WorldLoadContext
{
	InputSystem* inputSystem = nullptr;
	
	MeshManager*     meshManager     = nullptr;
	MaterialManager* materialManager = nullptr;
	TextureManager*  textureManager  = nullptr;
};

struct WorldUpdateContext
{
	Viewport* viewport = nullptr;
	double deltaTime = 0.0;
	
	InputSystem* inputSystem = nullptr;
	std::vector<ActionEvent> actionEventQueue;
};

class World : public IReflectedType
{
	friend class Application;
	WV_REFLECT_TYPE( World, IReflectedType )
public:
	World();
	virtual ~World();

	MeshManager*     getMeshManager()     const { return m_meshManager; }
	MaterialManager* getMaterialManager() const { return m_materialManager; }
	TextureManager*  getTextureManager()  const { return m_textureManager; }

	Viewport* getViewport() const                { return m_viewport; }
	void      setViewport( Viewport* _viewport ) { m_viewport = _viewport; }

	void addEntity( Entity* _entity ) {
		m_entities.push_back( _entity );
	}

	Entity* createEntity( const std::string& _name = "" );

	template<typename Ty>
	void addComponent( Entity* _entity, const Ty& _component ) {
		m_ecsEngine->addComponent<Ty>( _entity, _component );
	}

	template<typename Ty>
	void removeComponent( Entity* _entity ) {
		m_ecsEngine->removeComponent<Ty>( _entity );
	}

	template<typename Ty>
	Ty* addSystem() {
		return m_ecsEngine->addSystem<Ty>();
	}

	template<typename Ty>
	Ty* getSystem() {
		return m_ecsEngine->getSystem<Ty>();
	}

protected:
	virtual void onSceneCreate() { }
	virtual void onSetupInput( InputSystem* _inputSystem ) { }
		
	Viewport* m_viewport = nullptr;
	ECSEngine* m_ecsEngine = nullptr;

	std::vector<Entity*> m_entities;

	MeshManager*     m_meshManager     = nullptr;
	MaterialManager* m_materialManager = nullptr;
	TextureManager*  m_textureManager  = nullptr;
};

}