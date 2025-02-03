#pragma once

#include <string>
#include <vector>

#include <wv/Scene/SceneObject.h>
#include <wv/Reflection/Reflection.h>

namespace wv
{

class IComponentInterface;

class Entity : public IEntity
{
public:
	Entity( const UUID& _uuid, const std::string& _name );

	~Entity() { 
		for ( size_t i = 0; i < m_components.size(); i++ )
			delete m_components[ i ];

		m_components.clear();
	}

	template<typename _Ty, typename... _Args>
	void addComponent( _Args... _args ) {
		static_assert( std::is_base_of_v<IComponentInterface, _Ty>, "Type must be a component" );

		_Ty* comp = new _Ty{ _args... };
		comp->registerUpdatable();
		m_components.push_back( comp );

	}

	static Entity* parseInstance( sParseData& _data ) {
		auto& json = _data.json;
		wv::UUID    uuid = json[ "uuid" ].int_value();
		std::string name = json[ "name" ].string_value();

		return new Entity( uuid, name );
	}

	template<typename _Ty>
	void getComponent() { }

	std::vector<IComponentInterface*> m_components{};

protected:

	virtual void onLoadImpl( void ) { };
	virtual void onUnloadImpl( void ) { };

	virtual void onCreateImpl ( void ) { };
	virtual void onDestroyImpl( void ) { };

	virtual void onUpdate( double _deltaTime ) { };
	virtual void drawImpl( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) { };


};

}