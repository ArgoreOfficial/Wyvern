#pragma once

#include <wv/entity/ecs.h>
#include <wv/entity/world.h>

#include <nlohmann/json.hpp>

#include <vector>
#include <functional>

namespace wv {

template<typename Ty>
static void to_json( nlohmann::json& _json, const Vector3<Ty>& _vec ) {
	_json = std::vector<Ty>{ _vec.x, _vec.y, _vec.z };
}

template<typename Ty>
static void from_json( const nlohmann::json& _json, Vector3<Ty>& _vec ) {
	_json.at( 0 ).get_to( _vec.x );
	_json.at( 1 ).get_to( _vec.y );
	_json.at( 2 ).get_to( _vec.z );
}

template<typename Ty>
static void to_json( nlohmann::json& _json, const Rotor<Ty>& _rot ) {
	_json = std::vector<Ty>{ _rot.s, _rot.xy, _rot.xz, _rot.yz };
}

template<typename Ty>
static void from_json( const nlohmann::json& _json, Rotor<Ty>& _rot ) {
	_json.at( 0 ).get_to( _rot.s );
	_json.at( 1 ).get_to( _rot.xy );
	_json.at( 2 ).get_to( _rot.xz );
	_json.at( 3 ).get_to( _rot.yz );
}

static void to_json( nlohmann::json& _json, const Transform& _tfm ) {
	_json = nlohmann::json{
		{ "pos", _tfm.position },
		{ "rot", _tfm.rotation },
		{ "scl", _tfm.scale }
	};
}

static void from_json( const nlohmann::json& _json, Transform& _tfm ) {
	_json.at( "pos" ).get_to( _tfm.position );
	_json.at( "rot" ).get_to( _tfm.rotation );
	_json.at( "scl" ).get_to( _tfm.scale );
}

class WorldSerializer
{
public:
	WorldSerializer( wv::ECSEngine* _ecs ) : m_ecs{ _ecs } { };

	nlohmann::json serializeComponents() {
		std::vector<nlohmann::ordered_json> components;
		for ( auto& func : m_componentToJsonFuncs )
		{
			nlohmann::json j = func();
			if ( !j.at( "comps" ).empty() )
				components.push_back( j );
		}

		return components;
	}

	void deserializeComponents( int _typeIndex, World* _world, const nlohmann::json& _json ) {
		if ( !m_createComponentFromJsonFuncs.contains( _typeIndex ) )
			return;

		m_createComponentFromJsonFuncs.at( _typeIndex )( _world, _json );
	}

	template<typename Ty>
	void addComponentFunction();

private:
	wv::ECSEngine* m_ecs;

	std::vector<std::function<nlohmann::json()>> m_componentToJsonFuncs;
	std::unordered_map<int, std::function<void( World* _world, const nlohmann::json& _json )>> m_createComponentFromJsonFuncs;

};

template<typename Ty>
inline void WorldSerializer::addComponentFunction()
{
	m_componentToJsonFuncs.push_back(
		[ this ]() -> nlohmann::json
		{
			std::vector<nlohmann::json> meshComponents{};

			std::vector<wv::Archetype*> archetypes = m_ecs->getMatchingArchetypes( m_ecs->getComponentBitset<Ty>() );
			for ( wv::Archetype* arch : archetypes )
			{
				auto comps = arch->getComponents<Ty>();
				auto ents = arch->getEntities();

				for ( size_t i = 0; i < arch->getNumEntities(); i++ )
				{
					if ( !ents[ i ]->getShouldSerialize() ) // do not serialize components on this entity
						continue;

					meshComponents.push_back(
						nlohmann::json{
							{ "entity", (uint64_t)ents[ i ]->getID() },
							{ "data", comps[ i ] }
						}
					);
				}
			}

			return nlohmann::ordered_json{
				{ "index", wv::ECSEngine::ComponentTypeDef<Ty>::index },
				{ "comps", meshComponents }
			};
		} );

	int index = wv::ECSEngine::ComponentTypeDef<Ty>::index;
	m_createComponentFromJsonFuncs[ index ] = 
		[]( World* _world, const nlohmann::json& _json ) // _json here is the { "comps" : [] } array
		{
			for ( auto& j : _json )
			{
				uint64_t entityID = 0;
				j[ "entity" ].get_to( entityID );

				Entity* ent = _world->getEntityFromID( entityID );
				if ( !ent )
					continue;

				Ty comp;
				j[ "data" ].get_to( comp );

				_world->addComponent<Ty>( ent, comp );
			}
		};
}
}