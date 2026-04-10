#pragma once

#include <wv/updatable.h>

#include <vector>
#include <bitset>

namespace wv {

class Archetype;

struct ArchetypeConfig;
struct WorldUpdateContext;

class ISystem : public IUpdatable
{
	friend class ECSEngine;
	friend class World;

public:
	std::vector<Archetype*>& getArchetypes() {
		return m_archetypes;
	}

	std::bitset<256> getArchetypeBitmask() const { return m_archetypeBitmask; }
	bool matchesBitmask( std::bitset<256> _bitmask ) const {
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
	std::bitset<256> m_archetypeBitmask{};
	std::vector<Archetype*> m_archetypes;
};

}