#pragma once

#include <wv/updatable.h>
#include <wv/types.h>

#include <vector>

namespace wv {

class Archetype;

struct ArchetypeConfig;
struct WorldUpdateContext;

class ISystem : public IUpdatable
{
	friend class ECSEngine;
	friend class World;

public:
	virtual ~ISystem() { }

	std::vector<Archetype*>& getArchetypes() {
		return m_archetypes;
	}

	ComponentBitmask getArchetypeBitmask() const { return m_archetypeBitmask; }
	bool matchesBitmask( ComponentBitmask _bitmask ) const {
		return ( m_archetypeBitmask & _bitmask ) == m_archetypeBitmask;
	}

	virtual void configure( ArchetypeConfig& _config ) = 0;

	virtual void onComponentAdded( Archetype* _archetype, size_t _index ) { }
	virtual void onComponentRemoved( Archetype* _archetype, size_t _index ) { }

protected:
	double deltaTime = 0.0;
	double physicsDeltaTime = 0.0;
	WorldUpdateContext* updateContext = nullptr;

private:
	ComponentBitmask m_archetypeBitmask{};
	std::vector<Archetype*> m_archetypes;
};

}