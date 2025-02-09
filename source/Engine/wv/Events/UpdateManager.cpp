#include "UpdateManager.h"

#include <wv/Engine/Engine.h>
#include <wv/Engine/ApplicationState.h>

#include <wv/JobSystem/JobSystem.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::IUpdatable::~IUpdatable()
{
	getAppState()->getUpdateManager()->unregisterUpdatable( this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IUpdatable::_registerUpdatable()
{
	getAppState()->getUpdateManager()->registerUpdatable( this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::registerUpdatable( IUpdatable* _pUpdatable )
{
	m_registerQueue.push( _pUpdatable );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::unregisterUpdatable( IUpdatable* _pUpdatable )
{
	m_unregisterQueue.push( _pUpdatable );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::_registerUpdatable( IUpdatable* _pUpdatable, IUpdatable::FunctionFlags _flags )
{
	if ( _flags & IUpdatable::FunctionFlags::kOnConstruct )
		m_onConstruct.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnDestruct )
		m_onDestruct.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnEnter )
		m_onEnter.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnExit )
		m_onExit.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnUpdate )
		m_onUpdate.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnPhysicsUpdate )
		m_onPhysicsUpdate.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnDraw )
		m_onDraw.insert( _pUpdatable );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::_unregisterUpdatable( IUpdatable* _pUpdatable )
{
	m_onConstruct.erase( _pUpdatable );
	m_onDestruct.erase( _pUpdatable );
	
	m_onEnter.erase( _pUpdatable );
	m_onExit.erase( _pUpdatable );
	
	if ( m_onUpdate.contains( _pUpdatable ) )
		m_onUpdate.erase( _pUpdatable );
	
	if ( m_onPhysicsUpdate.contains( _pUpdatable ) )
		m_onPhysicsUpdate.erase( _pUpdatable );

	if ( m_onDraw.contains( _pUpdatable ) )
		m_onDraw.erase( _pUpdatable );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::_updateQueued( void )
{
	while( !m_unregisterQueue.empty() )
	{
		IUpdatable* u = m_unregisterQueue.front();
		m_unregisterQueue.pop();

		_unregisterUpdatable( u );
	}

	while( !m_registerQueue.empty() )
	{
		IUpdatable* u = m_registerQueue.front();
		m_registerQueue.pop();

		IUpdatable::FunctionFlags flags = u->getFunctionFlags();
		_registerUpdatable( u, flags );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::onConstruct( void )
{
	struct JobData { IUpdatable* u; };

	Job::JobFunction_t fptr = []( void* _pData )
		{
			JobData* data = (JobData*)_pData;
			data->u->onConstruct();
		};

	_runJobs<JobData>( "comp_onConstruct", m_onConstruct.m_awaiting, fptr );

	m_onConstruct.complete();
	m_onDestruct.reset();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::onDestruct( void )
{
	struct JobData { IUpdatable* u; };

	Job::JobFunction_t fptr = []( void* _pData )
		{
			JobData* data = (JobData*)_pData;
			data->u->onDestruct();
		};

	_runJobs<JobData>( "comp_onDestruct", m_onDestruct.m_awaiting, fptr );

	m_onDestruct.complete();
	m_onConstruct.reset();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::onEnter( void )
{
	struct JobData { IUpdatable* u; };

	Job::JobFunction_t fptr = []( void* _pData )
		{
			JobData* data = (JobData*)_pData;
			data->u->onEnter();
		};

	_runJobs<JobData>( "comp_onEnter", m_onEnter.m_awaiting, fptr );

	m_onEnter.complete();
	m_onExit.reset();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::onExit( void )
{
	struct JobData { IUpdatable* u; };

	Job::JobFunction_t fptr = []( void* _pData )
		{
			JobData* data = (JobData*)_pData;
			data->u->onExit();
		};

	_runJobs<JobData>( "comp_onExit", m_onExit.m_awaiting, fptr );

	m_onExit.complete();
	m_onEnter.reset();
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::onUpdate( double _deltaTime )
{
	struct JobData
	{
		IUpdatable* u;
		double dt;
	};

	Job::JobFunction_t fptr = []( void* _pData )
		{
			JobData* data = (JobData*)_pData;
			data->u->onUpdate( data->dt );
		};

	_runJobs<JobData>( "comp_onUpdate", m_onUpdate, fptr, _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::onPhysicsUpdate( double _deltaTime )
{
	struct JobData
	{
		IUpdatable* u;
		double dt;
	};

	Job::JobFunction_t fptr = []( void* _pData )
		{
			JobData* data = (JobData*)_pData;
			data->u->onPhysicsUpdate( data->dt );
		};

	_runJobs<JobData>( "comp_onPhysicsUpdate", m_onPhysicsUpdate, fptr, _deltaTime );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::UpdateManager::onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device )
{
	struct JobData
	{
		IUpdatable* u;
		wv::iDeviceContext* context;
		wv::iLowLevelGraphics* device;
	};

	Job::JobFunction_t fptr = []( void* _pData )
		{
			JobData* data = (JobData*)_pData;
			data->u->onDraw( data->context, data->device );
		};

	_runJobs<JobData>( "comp_onDraw", m_onDraw, fptr, _context, _device );
}
