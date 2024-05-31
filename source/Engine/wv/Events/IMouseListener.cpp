#include "IMouseListener.h"

void wv::IMouseListener::invoke( MouseEvent _event )
{
	if ( !m_enabled )
		return;

	for ( int i = 0; i < m_hooks.size(); i++ )
		m_hooks[ i ]->onMouseEvent( _event );
}

void wv::IMouseListener::subscribeMouseEvents( void )
{
	IMouseListener::m_hooks.push_back( this );
}

void wv::IMouseListener::unsubscribeMouseEvents( void )
{
	for ( int i = 0; i < IMouseListener::m_hooks.size(); i++ )
	{
		if ( IMouseListener::m_hooks[ i ] == this )
		{
			IMouseListener::m_hooks.erase( IMouseListener::m_hooks.begin() + i );
			return;
		}
	}
}
