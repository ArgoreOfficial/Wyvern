#include "iMouseListener.h"

void wv::iMouseListener::invoke( sMouseEvent _event )
{
	for ( int i = 0; i < m_hooks.size(); i++ )
		m_hooks[ i ]->onMouseEvent( _event );
}

void wv::iMouseListener::subscribeMouseEvents( void )
{
	iMouseListener::m_hooks.push_back( this );
}

void wv::iMouseListener::unsubscribeMouseEvents( void )
{
	for ( int i = 0; i < iMouseListener::m_hooks.size(); i++ )
	{
		if ( iMouseListener::m_hooks[ i ] == this )
		{
			iMouseListener::m_hooks.erase( iMouseListener::m_hooks.begin() + i );
			return;
		}
	}
}
