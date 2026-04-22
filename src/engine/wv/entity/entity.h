#pragma once

#include <wv/math/transform.h>

#include <wv/reflection/reflection.h>
#include <wv/memory/memory.h>
#include <wv/types.h>

namespace wv {

class World;

struct Archetype;

class Entity final : wv::IReflectedType
{
	friend class World;

	WV_REFLECT_TYPE( Entity, IReflectedType )
public:
	Entity( const std::string& _debugName = "" ) : 
		m_ID{ wv::Math::randomU32() },
		m_debugName{ _debugName }
	{ }

	Entity( UUID _id, const std::string& _debugName = "" ) : 
		m_ID{ _id },
		m_debugName{ _debugName }
	{ }

	~Entity() { }

	UUID getID() const { return m_ID; }
	std::string getName() { return m_debugName; }

	Transform& getTransform() { return m_transform; }

	bool getDebugDisplayEnabled()          const { return m_debugDisplayEnabled; }
	void setDebugDisplayEnabled( bool _enabled ) { m_debugDisplayEnabled = _enabled; }

	bool getShouldSerialize()            const { return m_shouldSerialize; }
	void setShouldSerialize( bool _serialize ) { m_shouldSerialize = _serialize; }

	Archetype* archetype = nullptr;
	
private:
	UUID m_ID = 0;
	std::string m_debugName = "";

	bool m_shouldSerialize = true;
	bool m_debugDisplayEnabled = true;

	Transform m_transform;

private:
	WV_REFLECT_MEMBER( m_ID, "UUID" )
	WV_REFLECT_MEMBER( m_debugName, "DebugName" )
	WV_REFLECT_MEMBER( m_transform, "Transform" )
};

}