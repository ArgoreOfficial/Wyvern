#pragma once

#include <wv/Events/UpdateManager.h>

#include <vector>
#include <string>

namespace wv
{

class IComponentInterface
{
public:

};

template<typename _Ty>
class IComponent : public IComponentInterface, public IUpdatable
{
public:
	IComponent() :
		m_name{ _Ty::getName() }
	{

	}

	~IComponent() {
		IUpdatable::destroyUpdatable();
	}

	static const char* getName() {
		return _Ty::getName();
	}

	virtual void onLoad  ( void ) override { };
	virtual void onUnload( void ) override { };

	virtual void onEnter( void ) override { };
	virtual void onExit ( void ) override { };
	
	virtual void onUpdate( double _deltaTime ) override { };
	virtual void onDraw  ( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device ) override { };

private:
	const std::string m_name;
};


}