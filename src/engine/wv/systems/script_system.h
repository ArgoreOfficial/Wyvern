#pragma once

#include <wv/entity/ecs.h>
#include <wv/entity/world.h>

#include <sol/sol.hpp>

#include <unordered_map>

namespace wv {

struct ScriptInternalData
{
	sol::environment env;
	UpdateEventType updateEvents;
};

class ScriptSystem : public ISystem
{
public:
	virtual void configure( ArchetypeConfig& _config ) override;

	virtual void onComponentAdded( Archetype* _archetype, size_t _index ) override;
	virtual void onComponentRemoved( Archetype* _archetype, size_t _index ) override;

	virtual void onInitialize() override;
	
	virtual void onPreUpdate() override;
	virtual void onUpdate() override;
	virtual void onPhysicsUpdate() override;

private:

	sol::state m_lua;

	std::unordered_map<uint64_t, ScriptInternalData> m_scriptInfos;
};

}