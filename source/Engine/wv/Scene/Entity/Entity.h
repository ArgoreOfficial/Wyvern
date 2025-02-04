#pragma once

#include <string>
#include <vector>

#include <wv/Memory/Memory.h>
#include <wv/Scene/SceneObject.h>
#include <wv/Reflection/Reflection.h>

namespace wv
{

class IComponent;

class Entity : public IEntity
{
public:
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

	static Entity* parseInstance( sParseData& _data ) {
		auto& json = _data.json;
		wv::UUID    uuid = json[ "uuid" ].int_value();
		std::string name = json[ "name" ].string_value();

		return WV_NEW( Entity, uuid, name );
	}

	template<typename _Ty>
	void getComponent()
	{
		throw std::runtime_error( "unimplemented" );
	}

protected:

	std::vector<IComponent*> m_components{};


	virtual void onConstructImpl( void ) { };
	virtual void onDeconstructImpl( void ) { };

	virtual void onEnterImpl ( void ) { };
	virtual void onExitImpl( void ) { };

	virtual void onUpdate( double _deltaTime ) { };
	virtual void onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) { };


};

}