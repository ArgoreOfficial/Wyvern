#pragma once

#include <vector>
#include <functional>

#include <wv/entity/ecs.h>

#include <nlohmann/json.hpp>

namespace wv {

template<typename Ty>
void to_json( nlohmann::json& _json, const Vector3<Ty>& _vec ) {
	_json = std::vector<Ty>{ _vec.x, _vec.y, _vec.z };
}

template<typename Ty>
void from_json( nlohmann::json& _json, const Vector3<Ty>& _vec ) {
	_json.at( 0 ).get_to( _vec.x );
	_json.at( 1 ).get_to( _vec.y );
	_json.at( 3 ).get_to( _vec.z );
}

class WorldSerializer
{
public:
	WorldSerializer( wv::ECSEngine* _ecs ) : m_ecs{ _ecs } { };

	nlohmann::json serializeComponents() {
		std::vector<nlohmann::ordered_json> components;
		for ( auto& func : m_componentToJsonFuncs )
			components.push_back( func() );

		return components;
	}

	template<typename Ty>
	void addComponentFunction();

private:
	wv::ECSEngine* m_ecs;

	std::vector<std::function<nlohmann::json()>> m_componentToJsonFuncs;

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
}
}