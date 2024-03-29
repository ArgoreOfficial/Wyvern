#include "iInputListener.h"

void wv::iInputListener::invoke( sInputEvent _event )
{
	for ( int i = 0; i < m_hooks.size(); i++ )
		m_hooks[ i ]->onInputEvent( _event );
}

void wv::iInputListener::subscribeInputEvent( void )
{
	iInputListener::m_hooks.push_back( this );
}

void wv::iInputListener::unsubscribeInputEvent( void )
{
	for ( int i = 0; i < iInputListener::m_hooks.size(); i++ )
	{
		if ( iInputListener::m_hooks[ i ] == this )
		{
			iInputListener::m_hooks.erase( iInputListener::m_hooks.begin() + i );
			return;
		}
	}
}
