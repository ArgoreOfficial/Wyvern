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

struct Viewport;

struct WorldUpdateContext
{
	InputSystem* inputSystem = nullptr;
	std::vector<ActionEvent> actionEventQueue;
};

enum UpdateEventType
{
	UpdateEvent_Initialize,
	UpdateEvent_Shutdown,

	UpdateEvent_PreUpdate,
	UpdateEvent_Update,
	UpdateEvent_PostUpdate,
	
	UpdateEvent_PhysicsUpdate,

	UpdateEvent_DebugRender,  // for world debug rendering, such as lines, icons, and text
	UpdateEvent_EditorRender, // for editor interface and debug windows

	UpdateEvent_Render
};

class World
{
	friend class Application;

public:
	World();
	virtual ~World();

	void unload( bool _destroyPersistent );
	
	Viewport* getViewport() const                { return m_viewport; }
	void      setViewport( Viewport* _viewport ) { m_viewport = _viewport; }

	Entity* createEntity ( const std::string& _name = "" );
	void    destroyEntity( Entity* _entity );

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
	int registerComponentType( const std::string& _name = "Unnamed Component", bool _serialize = true ) {
		int index = m_ecsEngine->registerComponentType<Ty>();

		EditorComponentInfo info{};
		info.name = _name;
		info.addComponentFunction    = []( World* _world, Entity* _entity ) { _world->addComponent<Ty>( _entity, Ty{} ); };
		info.removeComponentFunction = []( World* _world, Entity* _entity ) { _world->removeComponent<Ty>( _entity ); };
		
		if ( _serialize )
		{
			info.serializeComponents =
				[this]() -> nlohmann::json
				{
					std::vector<nlohmann::json> comps;
				
					for ( Archetype* arch : m_ecsEngine->getMatchingArchetypes( m_ecsEngine->getComponentBitset<Ty>() ) )
					{
						auto& components = arch->getComponents<Ty>();
						auto& entities = arch->getEntities();

						for ( size_t i = 0; i < arch->getNumEntities(); i++ )
						{
							if ( !entities[ i ]->getShouldSerialize() )
								continue;
						
							comps.push_back(
								nlohmann::json{
									{ "data", Serialize::toJson( components[ i ] ) },
									{ "entity", (uint64_t)entities[ i ]->getID() }
								}
							);
						}
					}

					if ( comps.empty() )
						return {};

					return {
						{ "index", ECSEngine::ComponentTypeDef<Ty>::index },
						{ "comps", comps }
					};
				};

			info.deserializeComponents =
				[ this ]( const nlohmann::json& _json )
				{
					for ( auto& v : _json )
					{
						if ( !v.contains( "entity" ) )
							continue;

						uint64_t entityID = v.at( "entity" );
						Entity* entity = getEntityFromID( entityID );

						if ( !entity )
							continue;

						Ty comp;
						Serialize::fromJson( v[ "data" ], comp );
						addComponent<Ty>( entity, comp );
					}
				};
		}

		m_editorComponentInfos.emplace( index, info );

		return index;
	}

	std::vector<int> getRegisteredComponents() const {
		std::vector<int> vec;
		for ( auto& [k, v] : m_editorComponentInfos )
			vec.push_back( k );
		return vec;
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
		compChange.type = ComponentChange::ComponentChangeType_Add;
		compChange.entity = _entity;
		compChange.componentTypeIndex = ECSEngine::ComponentTypeDef<Ty>::index;
		compChange.callback = [ this, _entity, _component ]() { m_ecsEngine->addComponent<Ty>( _entity, _component ); };
		
		m_componentChangeQueue.push_back( compChange );
	}

	template<typename Ty>
	void removeComponent( Entity* _entity ) {
		ComponentChange compChange{};
		compChange.type = ComponentChange::ComponentChangeType_Remove;
		compChange.entity = _entity;
		compChange.componentTypeIndex = ECSEngine::ComponentTypeDef<Ty>::index;
		compChange.callback = [ this, _entity ]() { m_ecsEngine->removeComponent<Ty>( _entity ); };

		m_componentChangeQueue.push_back( compChange );
	}

	void addComponent( int _typeIndex, Entity* _entity ) {
		if ( !m_editorComponentInfos.contains( _typeIndex ) )
			return;

		m_editorComponentInfos.at( _typeIndex ).addComponentFunction( this, _entity );
	}

	void removeComponent( int _typeIndex, Entity* _entity ) {
		if ( !m_editorComponentInfos.contains( _typeIndex ) )
			return;

		m_editorComponentInfos.at( _typeIndex ).removeComponentFunction( this, _entity );
	}

	std::string getComponentName( int _typeIndex ) const {
		if ( !m_editorComponentInfos.contains( _typeIndex ) )
			return "";
		return m_editorComponentInfos.at( _typeIndex ).name;
	}

	void removeAllComponents( Entity* _entity ) {
		ComponentChange compChange{};
		compChange.type = ComponentChange::ComponentChangeType_RemoveAll;
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

	Viewport* m_viewport = nullptr;

	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_destroyEntities;
	std::unordered_set<IUpdatable*> m_updatables;

	WorldUpdateContext m_updateContext{};

	bool m_isInEditorState = false;

private:
	ECSEngine* m_ecsEngine = nullptr;

	struct ComponentChange
	{
		enum ComponentChangeType
		{
			ComponentChangeType_Add,
			ComponentChangeType_Remove,
			ComponentChangeType_RemoveAll
		};
		ComponentChangeType type;
		Entity* entity;
		int componentTypeIndex;
		std::function<void()> callback;
	};

	struct EditorComponentInfo
	{
		std::string name;
		std::function<void( World*, Entity* )> addComponentFunction;
		std::function<void( World*, Entity* )> removeComponentFunction;
		std::function<nlohmann::json()> serializeComponents;
		std::function<void( const nlohmann::json& )> deserializeComponents;
	};

	std::unordered_map<int, EditorComponentInfo> m_editorComponentInfos;

	void updateComponentChanges();
	void checkComponentAddChanges( std::bitset<256> _oldBitmask, std::bitset<256> _newBitmask, Entity* _change );
	void checkComponentRemoveChanges( std::bitset<256> _oldBitmask, std::bitset<256> _newBitmask, Entity* _change );

	std::vector<ComponentChange> m_componentChangeQueue;
};

}