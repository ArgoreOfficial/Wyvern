#pragma once

#include <string>
#include <vector>

#include <wv/memory/memory.h>
#include <wv/scene/scene_object.h>
#include <wv/reflection/reflection.h>

#include <wv/scene/component/script_component.h>

namespace wv
{

class IComponent;

class Entity : public IEntity
{
public:
	 Entity();
	 Entity( const UUID& _uuid, const std::string& _name );
	~Entity();

	

	template<typename _Ty, typename... _Args>
	void addComponent( _Args... _args ) {
		static_assert( std::is_base_of_v<IComponent, _Ty>, "Type must be a component" );

		_Ty* comp = WV_NEW( _Ty, _args... );
		comp->_registerUpdatable();
		comp->m_pParent = this;
		m_components.push_back( comp );

	}

	static Entity* parseInstance( ParseData& _data ) {
		auto& json = _data.json;
		wv::UUID    uuid = json[ "uuid" ].int_value();
		std::string name = json[ "name" ].string_value();
		
		Entity* entity = WV_NEW( Entity, uuid, name );
		entity->addComponent<ScriptComponent>();

		return entity;
	}

	template<typename _Ty>
	void getComponent()
	{
		throw std::runtime_error( "unimplemented" );
	}

protected:

	std::vector<IComponent*> m_components{};

public:
	virtual FunctionFlags getFunctionFlags() override { return FunctionFlags::kNone; };

	virtual void onConstructImpl  ( void ) override { };
	virtual void onDeconstructImpl( void ) override { };
	virtual void onEnterImpl( void ) override { };
	virtual void onExitImpl ( void ) override { };
	

};

}