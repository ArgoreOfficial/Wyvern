#include "cLayerStack.h"

void WV::cLayerStack::start()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->start();
	}
}

void WV::cLayerStack::update( double _deltaTime )
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->update( _deltaTime );
	}
}

void WV::cLayerStack::draw3D()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->draw3D();
	}
}

void WV::cLayerStack::draw2D()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->draw2D( );
	}
}

void WV::cLayerStack::drawUI()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->drawUI();
	}
}

