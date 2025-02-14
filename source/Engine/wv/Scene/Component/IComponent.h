#pragma once

#include <wv/Events/UpdateManager.h>

#include <vector>
#include <string>

namespace wv
{

class Entity;

class IComponent : public IUpdatable
{
public:
	friend class Entity;

	IComponent( const std::string _name ):
		m_name{ _name }
	{

	}

	const std::string getName() const { 
		return m_name; 
	}

	Entity* getParent() {
		return m_pParent;
	}

	virtual void onConstruct  ( void ) override { };
	virtual void onDestruct( void ) override { };

	virtual void onEnter( void ) override { };
	virtual void onExit ( void ) override { };
	
	virtual void onUpdate( double /*_deltaTime*/ ) override { };
	virtual void onDraw  ( wv::iDeviceContext* /*_context*/, wv::iLowLevelGraphics* /*_device*/ ) override { };

private:
	const std::string m_name;

	Entity* m_pParent;
};


}