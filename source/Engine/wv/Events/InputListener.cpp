#include "InputListener.h"

///////////////////////////////////////////////////////////////////////////////////////

std::vector<wv::iInputListener*> wv::iInputListener::m_hooks{};
bool wv::iInputListener::m_enabled = true;

///////////////////////////////////////////////////////////////////////////////////////

void wv::iInputListener::invoke( sInputEvent _event )
{
	for ( int i = 0; i < m_hooks.size(); i++ )
		m_hooks[ i ]->onInputEvent( _event );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iInputListener::subscribeInputEvent( void )
{
	iInputListener::m_hooks.push_back( this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iInputListener::unsubscribeInputEvent( void )
{
	auto it = std::find( iInputListener::m_hooks.begin(), iInputListener::m_hooks.end(), this );
	if ( it != iInputListener::m_hooks.end() )
		iInputListener::m_hooks.erase( it );
}
