#include "WindowListener.h"

///////////////////////////////////////////////////////////////////////////////////////

std::vector<wv::iWindowListener*> wv::iWindowListener::m_hooks{};

bool wv::iWindowListener::m_enabled = true;

void wv::iWindowListener::invoke( sWindowEvent _event )
{
	for ( int i = 0; i < m_hooks.size(); i++ )
		m_hooks[i]->onWindowEvent( _event );
}

void wv::iWindowListener::setEnabled( bool _enabled )
{
	m_enabled = _enabled;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iWindowListener::subscribeWindowEvents( void )
{
	iWindowListener::m_hooks.push_back( this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iWindowListener::unsubscribeWindowEvents( void )
{
	auto it = std::find( iWindowListener::m_hooks.begin(), iWindowListener::m_hooks.end(), this );
	if ( it != iWindowListener::m_hooks.end() )
		iWindowListener::m_hooks.erase( it );
}
