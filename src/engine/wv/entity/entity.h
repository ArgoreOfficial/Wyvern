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

	~Entity() { 
		if ( m_parent )
			m_parent->removeChild( this );

		while( !m_children.empty() )
			removeChild( m_children.back() );
	}

	UUID getID() const { return m_ID; }
	std::string getName() { return m_debugName; }

	void update( Entity* _parent, bool _recalculateMatrix = true ) {
		bool recalc = m_transform.update( _parent ? &_parent->getTransform() : nullptr, _recalculateMatrix );

		for ( Entity* child : m_children )
			child->update( this, recalc || _recalculateMatrix );
	}

	Transform& getTransform() { return m_transform; }

	std::vector<Entity*> getChildren() const { return m_children; }
	Entity* getParent() const { return m_parent; }

	inline void addChild( Entity* _child ) {
		if ( _child == nullptr )
			return;

		for ( auto& child : m_children ) // if child is already added
			if ( child == _child )
				return;

		m_children.push_back( _child );
		_child->m_parent = this;
	}

	inline void removeChild( Entity* _child ) {
		if ( _child == nullptr )
			return;

		for ( size_t i = 0; i < m_children.size(); i++ )
		{
			if ( m_children[ i ] != _child )
				continue;

			m_children.erase( m_children.begin() + i );
			return;
		}
	}

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

	Entity* m_parent = nullptr;
	std::vector<Entity*> m_children;

private:
	WV_REFLECT_MEMBER( m_ID, "UUID" )
	WV_REFLECT_MEMBER( m_debugName, "DebugName" )
	WV_REFLECT_MEMBER( m_transform, "Transform" )
};

}