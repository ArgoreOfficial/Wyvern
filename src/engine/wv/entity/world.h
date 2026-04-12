#pragma once

#include <wv/entity/ecs.h>
#include <wv/input/input_system.h>
#include <wv/reflection/reflection.h>

namespace wv {

class Entity;
class IUpdatable;
class InputSystem;

class MeshManager;
class MaterialManager;
class TextureManager;

struct Viewport;

struct WorldUpdateContext
{
	InputSystem* inputSystem = nullptr;
	std::vector<ActionEvent> actionEventQueue;
};

enum UpdateEventType
{
	UpdateEvent_initialize,
	UpdateEvent_shutdown,

	UpdateEvent_preUpdate,
	UpdateEvent_update,
	UpdateEvent_postUpdate,
	
	UpdateEvent_physicsUpdate,

	UpdateEvent_debugRender,  // for world debug rendering, such as lines, icons, and text
	UpdateEvent_editorRender, // for editor interface and debug windows

	UpdateEvent_render
};

class World : public IReflectedType
{
	friend class Application;
	WV_REFLECT_TYPE( World, IReflectedType )
public:
	World();
	virtual ~World();

	void destroyAllEntities();
	
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
	int registerComponentType() {
		return m_ecsEngine->registerComponentType<Ty>();
	}

	template<typename Ty>
	void addComponent( Entity* _entity, const Ty& _component ) {
		ComponentChange compChange{};
		compChange.type = ComponentChange::ComponentChangeType_add;
		compChange.entity = _entity;
		compChange.componentTypeIndex = ECSEngine::ComponentTypeDef<Ty>::index;
		compChange.callback = [ this, _entity, _component ]() { m_ecsEngine->addComponent<Ty>( _entity, _component ); };
		
		m_componentChangeQueue.push_back( compChange );
	}

	template<typename Ty>
	void removeComponent( Entity* _entity ) {
		ComponentChange compChange{};
		compChange.type = ComponentChange::ComponentChangeType_remove;
		compChange.entity = _entity;
		compChange.componentTypeIndex = ECSEngine::ComponentTypeDef<Ty>::index;
		compChange.callback = [ this, _entity ]() { m_ecsEngine->removeComponent<Ty>( _entity ); };

		m_componentChangeQueue.push_back( compChange );
	}

	void removeAllComponents( Entity* _entity ) {
		ComponentChange compChange{};
		compChange.type = ComponentChange::ComponentChangeType_removeAll;
		compChange.entity = _entity;
		compChange.callback = [ this, _entity ]() { m_ecsEngine->removeAllComponents( _entity ); };

		m_componentChangeQueue.push_back( compChange );
	}

	template<typename Ty>
	Ty* addSystem() {
		Ty* s = m_ecsEngine->addSystem<Ty>();
		insertUpdatable( (IUpdatable*)s );
		return s;
	}

	template<typename Ty>
	void removeSystem() {
		WV_ASSERT( false );
		//eraseUpdatable()
	}

	template<typename Ty>
	Ty* getSystem() {
		return m_ecsEngine->getSystem<Ty>();
	}

	void updateFrameData( double _deltaTime, double _physicsDeltaTime );
	void dispatchUpdateMessage( UpdateEventType _type );

	void insertUpdatable( IUpdatable* _updatable ) {
		if ( m_updatables.contains( _updatable ) )
			return;

		m_updatables.insert( _updatable );
	}

	void eraseUpdatable( IUpdatable* _updatable ) {
		if ( !m_updatables.contains( _updatable ) )
			return;

		m_updatables.erase( _updatable );
	}

protected:
	virtual void onSceneCreate() { }
	virtual void onSetupInput( InputSystem* _inputSystem ) { }
		
	Viewport* m_viewport = nullptr;

	std::vector<Entity*> m_entities;
	std::unordered_set<IUpdatable*> m_updatables;

	MeshManager*     m_meshManager     = nullptr;
	MaterialManager* m_materialManager = nullptr;
	TextureManager*  m_textureManager  = nullptr;
	
	WorldUpdateContext m_updateContext{};

private:
	ECSEngine* m_ecsEngine = nullptr;

	struct ComponentChange
	{
		enum ComponentChangeType
		{
			ComponentChangeType_add,
			ComponentChangeType_remove,
			ComponentChangeType_removeAll
		};
		ComponentChangeType type;
		Entity* entity;
		int componentTypeIndex;
		std::function<void()> callback;
	};

	void updateComponentChanges();
	void checkComponentAddChanges( std::bitset<256> _oldBitmask, std::bitset<256> _newBitmask, Entity* _change );
	void checkComponentRemoveChanges( std::bitset<256> _oldBitmask, std::bitset<256> _newBitmask, Entity* _change );

	std::vector<ComponentChange> m_componentChangeQueue;
};

}