#pragma once

#include <wv/entity/ecs.h>
#include <wv/entity/world.h>

#include <nlohmann/json.hpp>
#include <imgui/imgui.h>

#include <vector>
#include <string>
#include <functional>

namespace wv {

struct ISerializeMemberTypeInfo
{
	std::string name;
	std::string displayName;
	size_t offset;
	virtual std::string format( void* _class ) = 0;
};

template<typename Ty>
struct SerializeMemberTypeInfo : ISerializeMemberTypeInfo
{
	virtual std::string format( void* _class ) override {
		WV_ASSERT( _class != nullptr );

		Ty* v = reinterpret_cast<Ty*>( (char*)_class + offset );
		return std::format( "{}", *v );
	}
};

struct SerializeInfo
{
	template <typename C, typename M>
	constexpr void registerMember( M C::* _p, const std::string& _name, const std::string& _displayName = "" )
	{
		SerializeMemberTypeInfo<M>* member = new SerializeMemberTypeInfo<M>();
		member->name = _name;
		member->offset = offset_of2( _p );

		if ( _displayName == "" )
			member->displayName = _name;
		else
			member->displayName = _displayName;

		members.push_back( (ISerializeMemberTypeInfo*)member );
	}

	bool serialize = true;
	std::string name;
	std::vector<ISerializeMemberTypeInfo*> members;

	std::function<nlohmann::json()> toJsonFunc;
	std::function<void( World* _world, const nlohmann::json& _json )> createFromJsonFunc;
};

template<typename Ty>
static void serialize( SerializeInfo& _info ) {
	_info.serialize = false; // specialization is required
}

template<typename Ty>
static void to_json( nlohmann::json& _json, const Vector3<Ty>& _vec ) {
	_json = std::vector<Ty>{ _vec.x, _vec.y, _vec.z };
}

template<typename Ty>
static void from_json( const nlohmann::json& _json, Vector3<Ty>& _vec ) {
	     if ( _json.size() == 0 ) _vec = Vector3<Ty>{};
	else if ( _json.size() == 1 ) _vec = Vector3<Ty>{ _json[ 0 ], 0.0f, 0.0f };
	else if ( _json.size() == 2 ) _vec = Vector3<Ty>{ _json[ 0 ], _json[ 1 ], 0.0f };
	else if ( _json.size() == 3 ) _vec = Vector3<Ty>{ _json[ 0 ], _json[ 1 ], _json[ 2 ] };
}

template<typename Ty>
static void to_json( nlohmann::json& _json, const Rotor<Ty>& _rot ) {
	_json = std::vector<Ty>{ _rot.s, _rot.xy, _rot.xz, _rot.yz };
}

template<typename Ty>
static void from_json( const nlohmann::json& _json, Rotor<Ty>& _rot ) {
	if ( _json.size() != 0 )
	{
		_rot.s  = _json[ 0 ];
		_rot.xy = _json[ 1 ];
		_rot.xz = _json[ 2 ];
		_rot.yz = _json[ 3 ];
	}
}

static void to_json( nlohmann::json& _json, const Transform& _tfm ) {
	_json = nlohmann::json{
		{ "pos", _tfm.position },
		{ "rot", _tfm.rotation },
		{ "scl", _tfm.scale }
	};
}

static void from_json( const nlohmann::json& _json, Transform& _tfm ) {
	_tfm.position = _json.value<Vector3f>( "pos", {} );
	_tfm.rotation = _json.value<Rotorf>( "rot", {} );

	if ( !_json[ "scl" ].is_array() || _json[ "scl" ].size() == 0 )
		_tfm.scale = { 1.0f, 1.0f, 1.0f };
	else
	{
		_tfm.scale = _json[ "scl" ];

		     if ( _json[ "scl" ].size() == 1 ) _tfm.scale = { _tfm.scale.x, 1.0f, 1.0f };
		else if ( _json[ "scl" ].size() == 2 ) _tfm.scale = { _tfm.scale.x, _tfm.scale.y, 1.0f };
	}
}

class WorldSerializer
{
public:
	WorldSerializer( wv::ECSEngine* _ecs ) : m_ecs{ _ecs } { };

	nlohmann::json serializeComponents() {
		std::vector<nlohmann::ordered_json> components;
		for ( auto& [k, v] : m_serializeInfos )
		{
			nlohmann::json j = v.toJsonFunc();
			if ( !j.at( "comps" ).empty() )
				components.push_back( j );
		}

		return components;
	}

	void deserializeComponents( int _typeIndex, World* _world, const nlohmann::json& _json ) {
		if ( !m_serializeInfos.contains( _typeIndex ) )
			return;

		m_serializeInfos.at( _typeIndex ).createFromJsonFunc( _world, _json );
	}

	template<typename Ty>
	void addComponentFunction();

	bool hasSerializeInfo( int _typeIndex ) const { 
		return m_serializeInfos.contains( _typeIndex ); 
	}

	const SerializeInfo& getSerializeInfo( int _typeIndex ) const {
		return m_serializeInfos.at( _typeIndex );
	}

private:
	wv::ECSEngine* m_ecs;

	std::unordered_map<int, SerializeInfo> m_serializeInfos;

};

template<typename Ty>
inline void WorldSerializer::addComponentFunction()
{
	int index = wv::ECSEngine::ComponentTypeDef<Ty>::index;

	SerializeInfo info{};
	serialize<Ty>( info );

	if ( !info.serialize )
		return;

	info.toJsonFunc =
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
		};

	info.createFromJsonFunc =
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

	m_serializeInfos[ index ] = info;
}
}