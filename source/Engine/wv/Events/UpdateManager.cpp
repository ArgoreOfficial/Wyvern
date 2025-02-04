#include "UpdateManager.h"

#include <wv/Engine/Engine.h>
#include <wv/Engine/ApplicationState.h>

#include <wv/JobSystem/JobSystem.h>

wv::IUpdatable::~IUpdatable()
{
	getAppState()->getUpdateManager()->unregisterUpdatable( this );
}

void wv::IUpdatable::_registerUpdatable()
{
	getAppState()->getUpdateManager()->registerUpdatable( this );
}



void wv::UpdateManager::registerUpdatable( IUpdatable* _pUpdatable )
{
	m_registerQueue.push( _pUpdatable );
}

void wv::UpdateManager::unregisterUpdatable( IUpdatable* _pUpdatable )
{
	m_unregisterQueue.push( _pUpdatable );
}

void wv::UpdateManager::_registerUpdatable( IUpdatable* _pUpdatable, IUpdatable::FunctionFlags _flags )
{
	if ( _flags & IUpdatable::FunctionFlags::kOnConstruct )
		m_onConstruct.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnDestruct )
		m_onDeconstruct.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnEnter )
		m_onEnter.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnExit )
		m_onExit.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnUpdate )
		m_onUpdate.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnDraw )
		m_onDraw.insert( _pUpdatable );
}

void wv::UpdateManager::_unregisterUpdatable( IUpdatable* _pUpdatable )
{
	m_onConstruct.erase( _pUpdatable );
	m_onDeconstruct.erase( _pUpdatable );
	
	m_onEnter.erase( _pUpdatable );
	m_onExit.erase( _pUpdatable );
	
	if ( m_onUpdate.contains( _pUpdatable ) )
		m_onUpdate.erase( _pUpdatable );
	
	if ( m_onDraw.contains( _pUpdatable ) )
		m_onDraw.erase( _pUpdatable );
}

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

void wv::UpdateManager::onConstruct( void )
{
	for ( auto& u : m_onConstruct.m_awaiting )
		u->onConstruct();
	m_onConstruct.complete();
}

void wv::UpdateManager::onDestruct( void )
{
	for( auto& u : m_onDeconstruct.m_awaiting )
		u->onDestruct();
	
	m_onDeconstruct.complete();
}

void wv::UpdateManager::onEnter( void )
{
	for( auto& u : m_onEnter.m_awaiting )
		u->onEnter();
	
	m_onEnter.complete();
}

void wv::UpdateManager::onExit( void )
{
	for( auto& u : m_onExit.m_awaiting )
		u->onExit();
	
	m_onExit.complete();
}

void wv::UpdateManager::onUpdate( double _deltaTime )
{
	struct UpdateData
	{
		IUpdatable* u;
		double dt;
	};

	JobSystem* pJobSystem = cEngine::get()->m_pJobSystem;

	std::vector<UpdateData> userDatas{ m_onUpdate.size() };
	std::vector<Job*> jobs{};

	JobCounter* counter = nullptr;

	Job::JobFunction_t fptr = []( const Job* _job, void* _pData )
		{
			UpdateData* data = (UpdateData*)_pData;
			data->u->onUpdate( data->dt );
		};

	int i = 0;
	for( auto& u : m_onUpdate )
	{
		userDatas[ i ] = UpdateData{ u, _deltaTime };
		Job* job = pJobSystem->createJob( "comp_onUpdate", fptr, &counter, &userDatas[i]);
		jobs.push_back( job );
		i++;
	}

	pJobSystem->run( jobs.data(), jobs.size() );
	pJobSystem->waitForAndFreeCounter( &counter, 0 );

	//for ( auto& u : m_onUpdate )
	//	u->onUpdate( _deltaTime );
}

void wv::UpdateManager::onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device )
{
	for ( auto& u : m_onDraw )
		u->onDraw( _context, _device );
}
