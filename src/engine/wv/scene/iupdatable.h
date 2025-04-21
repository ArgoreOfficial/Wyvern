#pragma once

#include <wv/decl.h>
#include <wv/job/job_system.h>
#include <wv/engine.h>

#include <vector>
#include <unordered_set>
#include <queue>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

class IDeviceContext;
class IGraphicsDevice;
class Engine;
class IAppState;

///////////////////////////////////////////////////////////////////////////////////////

class IUpdatable
{
public:
	
	IUpdatable() { }
	virtual ~IUpdatable();

	void _registerUpdatable();
	
	enum FunctionFlags
	{
		kNone            = 0,
		kOnConstruct     = 1,
		kOnDestruct      = 1 << 1,
		kOnEnter         = 1 << 2,
		kOnExit          = 1 << 3,
		kOnUpdate        = 1 << 4,
		kOnPhysicsUpdate = 1 << 5,
		kOnDraw          = 1 << 6,
	};

	virtual FunctionFlags getFunctionFlags() = 0;

	bool hasEntered    ( void ) { return m_isEntered; }
	bool hasConstructed( void ) { return m_isConstructed; }

	void callOnConstruct( void ) { 
		if( hasConstructed() == false )
			this->onConstruct();
		m_isConstructed = true;
	}

	void callOnDestruct( void ) { 
		if( hasConstructed() )
			this->onDestruct();
		
		m_isConstructed = false;
	}

	void callOnEnter( void ) { 
		if( hasEntered() == false )
			this->onEnter();

		m_isEntered = true;
	}

	void callOnExit( void ) { 
		if( hasEntered() )
			this->onExit();

		m_isEntered = false;
	}

	void callOnUpdate       ( double _deltaTime ) { this->onUpdate       ( _deltaTime ); }
	void callOnPhysicsUpdate( double _deltaTime ) { this->onPhysicsUpdate( _deltaTime ); }

	void callOnDraw( wv::IDeviceContext* _context, wv::IGraphicsDevice* _device ) {
		this->onDraw( _context, _device );
	}

protected:
	virtual void onConstruct( void ) { }
	virtual void onDestruct ( void ) { }

	virtual void onEnter( void ) { }
	virtual void onExit ( void ) { }

	virtual void onUpdate       ( double /*_deltaTime*/ ) { }
	virtual void onPhysicsUpdate( double /*_deltaTime*/ ) { }

	virtual void onDraw( wv::IDeviceContext* /*_context*/, wv::IGraphicsDevice* /*_device */) { }

private:

	bool m_isEntered     = false;
	bool m_isConstructed = false;

};

WV_ENUM_BITWISE_OR( IUpdatable::FunctionFlags )

}