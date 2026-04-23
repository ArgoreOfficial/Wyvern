#pragma once

#include <wv/entity/ecs.h>
#include <wv/input/input_system.h>
#include <wv/reflection/reflection.h>

#include <filesystem>

#include <nlohmann/json.hpp>

namespace wv {

class Entity;
class IUpdatable;
class InputSystem;
class WorldSerializer;

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
	friend class WorldSerializer;

	WV_REFLECT_TYPE( World, IReflectedType )
public:
	World();
	virtual ~World();

	void unload( bool _destroyPersistent );
	
	Viewport* getViewport() const                { return m_viewport; }
	void      setViewport( Viewport* _viewport ) { m_viewport = _viewport; }

	Entity* createEntity( const std::string& _name = "" );

	Entity* getEntityFromID( UUID _id ) const {
		for ( Entity* e : m_entities )
			if ( e->getID() == _id )
				return e;
		return nullptr;
	}

	void load( const std::filesystem::path& _path );
	void save( const std::filesystem::path& _path );
	void reload( bool _reloadAssets = true );

	std::filesystem::path getPath() const { return m_path; }

	template<typename Ty>
	int registerComponentType() {
		return m_ecsEngine->registerComponentType<Ty>();
	}

	template<typename Ty>
	Ty& getComponent( Entity* _entity ) {
		Archetype* archetype = _entity->archetype;
		WV_ASSERT( archetype != nullptr );

		return archetype->getComponents<Ty>()[ archetype->getEntityIndex( _entity ) ];
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

	std::vector<ISystem*> getSystems() const { return m_ecsEngine->getSystems(); }

	void updateFrameData( double _deltaTime, double _physicsDeltaTime );
	void dispatchUpdateMessage( UpdateEventType _type );

	void insertUpdatable( IUpdatable* _updatable );
	void eraseUpdatable( IUpdatable* _updatable );

	void toggleEditorState() { m_isInEditorState = !m_isInEditorState; }

	bool isEditorState()  const { return m_isInEditorState; }
	bool isRuntimeState() const { return !m_isInEditorState; }

	// Sets the editor/runtime state
	void setEditorState( bool _editorState ) { m_isInEditorState = _editorState; }

	std::vector<Entity*> getActiveEntities() const {
		std::vector<Entity*> vec;
		for ( auto& e : m_entities )
			vec.push_back( e );
		return vec;
	}

protected:
	virtual void onSceneCreate() { }
	virtual void onSetupInput( InputSystem* _inputSystem ) { }
	
	std::filesystem::path m_path;
	WorldSerializer* m_serializer{};

	Viewport* m_viewport = nullptr;

	std::vector<Entity*> m_entities;
	std::unordered_set<IUpdatable*> m_updatables;

	WorldUpdateContext m_updateContext{};

	bool m_isInEditorState = false;

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