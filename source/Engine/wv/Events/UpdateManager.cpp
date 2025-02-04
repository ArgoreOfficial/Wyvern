#include "UpdateManager.h"

#include <wv/Engine/Engine.h>
#include <wv/Engine/ApplicationState.h>

wv::IUpdatable::~IUpdatable()
{
	getAppState()->getUpdateManager()->removeUpdatable( this );
}

void wv::IUpdatable::registerUpdatable()
{
	FunctionFlags flags = getFunctionFlags();
	getAppState()->getUpdateManager()->registerUpdatable(this, flags);
}

void wv::UpdateManager::registerUpdatable( IUpdatable* _pUpdatable, IUpdatable::FunctionFlags _flags )
{
	if ( _flags & IUpdatable::FunctionFlags::kOnConstruct )
		m_onConstruct.insert( _pUpdatable );

	if ( _flags & IUpdatable::FunctionFlags::kOnDeconstruct )
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

void wv::UpdateManager::removeUpdatable( IUpdatable* _pUpdatable )
{
	if ( m_onConstruct.contains( _pUpdatable ) )
		m_onConstruct.erase( _pUpdatable );

	if ( m_onDeconstruct.contains( _pUpdatable ) )
		m_onDeconstruct.erase( _pUpdatable );
	
	if ( m_onEnter.contains( _pUpdatable ) )
		m_onEnter.erase( _pUpdatable );
	
	if ( m_onExit.contains( _pUpdatable ) )
		m_onExit.erase( _pUpdatable );
	
	if ( m_onUpdate.contains( _pUpdatable ) )
		m_onUpdate.erase( _pUpdatable );
	
	if ( m_onDraw.contains( _pUpdatable ) )
		m_onDraw.erase( _pUpdatable );
}

void wv::UpdateManager::onConstruct( void )
{
	for ( auto& u : m_onConstruct )
		u->onConstruct();
}

void wv::UpdateManager::onDeconstruct( void )
{
	for ( auto& u : m_onDeconstruct )
		u->onDeconstruct();
}

void wv::UpdateManager::onEnter( void )
{
	for ( auto& u : m_onEnter )
		u->onEnter();
}

void wv::UpdateManager::onExit( void )
{
	for ( auto& u : m_onExit )
		u->onExit();
}

void wv::UpdateManager::onUpdate( double _deltaTime )
{
	for ( auto& u : m_onUpdate )
		u->onUpdate( _deltaTime );
}

void wv::UpdateManager::onDraw( wv::iDeviceContext* _context, wv::iLowLevelGraphics* _device )
{
	for ( auto& u : m_onDraw )
		u->onDraw( _context, _device );
}
