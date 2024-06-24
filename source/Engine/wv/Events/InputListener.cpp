#include "InputListener.h"

///////////////////////////////////////////////////////////////////////////////////////

void wv::IInputListener::invoke( InputEvent _event )
{
	for ( int i = 0; i < m_hooks.size(); i++ )
		m_hooks[ i ]->onInputEvent( _event );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IInputListener::subscribeInputEvent( void )
{
	IInputListener::m_hooks.push_back( this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IInputListener::unsubscribeInputEvent( void )
{
	for ( int i = 0; i < IInputListener::m_hooks.size(); i++ )
	{
		if ( IInputListener::m_hooks[ i ] == this )
		{
			IInputListener::m_hooks.erase( IInputListener::m_hooks.begin() + i );
			return;
		}
	}
}
