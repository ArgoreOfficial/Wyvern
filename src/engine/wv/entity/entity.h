#pragma once

#include <wv/math/transform.h>

#include <wv/reflection/reflection.h>
#include <wv/memory/memory.h>
#include <wv/types.h>

#include <wv/application.h>

#include <imgui/imgui.h>

namespace wv {

class World;

struct Archetype;

class Entity
{
	friend class World;
public:
	Entity( const std::string& _name = "" ) : 
		m_ID{ wv::Math::randomU64() },
		m_name{ _name }
	{ }

	Entity( UUID _id, const std::string& _name = "" ) :
		m_ID{ _id },
		m_name{ _name }
	{ }

	~Entity() { 
		if ( m_parent )
			m_parent->removeChild( this );

		while( !m_children.empty() )
			removeChild( m_children.back() );
	}

	UUID getID() const { return m_ID; }

	std::string getName()                     const { return m_name; }
	void        setName( const std::string& _name ) { m_name = _name; }

	void update( Entity* _parent, bool _recalculateMatrix = true ) {
		bool recalc = m_transform.update( _parent ? &_parent->getTransform() : nullptr, _recalculateMatrix );

		for ( Entity* child : m_children )
			child->update( this, recalc || _recalculateMatrix );
	}

	Transform& getTransform() { return m_transform; }

	std::vector<Entity*> getChildren() const { return m_children; }
	
	Entity* getParent() const { return m_parent; }
	void setParent( Entity* _newParent ) { 
		if ( m_parent )
			m_parent->removeChild( this );
		
		if( _newParent )
			_newParent->addChild( this );
	}

	bool isChildOf( Entity* _parent ) const {
		if ( m_parent == nullptr )
			return false;

		if ( m_parent == _parent )
			return true;

		return m_parent->isChildOf( _parent );
	}

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

			m_children[ i ]->m_parent = nullptr;
			m_children.erase( m_children.begin() + i );
			return;
		}
	}

	bool getDebugDisplayEnabled()          const { return m_debugDisplayEnabled; }
	void setDebugDisplayEnabled( bool _enabled ) { m_debugDisplayEnabled = _enabled; }

	bool getShouldSerialize()            const { return m_shouldSerialize; }
	void setShouldSerialize( bool _serialize ) { m_shouldSerialize = _serialize; }

	// Persistent entities don't get destroyed on world->destroyWorld()
	bool getIsPersistent() const { return m_isPersistent; }
	void setIsPersistent( bool _persistent ) { m_isPersistent = _persistent; }

	Archetype* archetype = nullptr;
	
private:
	UUID m_ID = 0;
	std::string m_name = "";

	bool m_isPersistent = false;
	bool m_shouldSerialize = true;
	bool m_debugDisplayEnabled = true;

	Transform m_transform;

	Entity* m_parent = nullptr;
	std::vector<Entity*> m_children;

public:
	static inline wv::Reflection reflection{
		wv::reflect( "id", &Entity::m_ID ),
		wv::reflect( "name", &Entity::m_name ),
		wv::reflect( "tfm", &Entity::m_transform )
	};
};


template<>
struct SerializeField<Entity*>
{
	static nlohmann::json toJson( const Entity*& _v ) { return (uint64_t)_v->getID(); }
	static void fromJson( const nlohmann::json& _json, Entity*& _v ) {
		if ( _json.is_null() )
			return;

		uint64_t v;
		_json.get_to( v );
		_v = wv::getEntityFromID( v );
	}
};


template<>
struct EditorField<Entity*>
{
	static bool imguiInput( const char* _label, Entity*& _v ) 
	{
		std::string entityName;
		if ( _v )
			entityName = _v->getName();
		else
			entityName = "None";
		
		ImGui::PushID( _label );
		ImGui::InputText( "", &entityName, ImGuiInputTextFlags_ReadOnly );
		ImGui::PopID();

		ImGui::PushID( "entity_editor_field" );
		if ( ImGui::BeginDragDropTarget() )
		{
			if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "WV_DND_ENTITY" ) )
			{
				// WV_ASSERT( payload->DataSize == sizeof( Entity* ) );
				_v = *(Entity**)payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::PopID();
		

		// ImGui::PushItemWidth( -1 );
		// bool edited = ImGui::InputText( label.c_str(), &path, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_ElideLeft);
		// ImGui::PopItemWidth();

		return false;
	}
};
}