#include "MouseListener.h"

///////////////////////////////////////////////////////////////////////////////////////

std::vector<wv::iMouseListener*> wv::iMouseListener::m_hooks{};
bool wv::iMouseListener::m_enabled = true;

void wv::iMouseListener::invoke( MouseEvent _event )
{
	for ( int i = 0; i < m_hooks.size(); i++ )
		m_hooks[ i ]->onMouseEvent( _event );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iMouseListener::subscribeMouseEvents( void )
{
	iMouseListener::m_hooks.push_back( this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::iMouseListener::unsubscribeMouseEvents( void )
{
	auto it = std::find( iMouseListener::m_hooks.begin(), iMouseListener::m_hooks.end(), this );
	if ( it != iMouseListener::m_hooks.end() )
		iMouseListener::m_hooks.erase( it );
}
