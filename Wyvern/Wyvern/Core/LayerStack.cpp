#include "LayerStack.h"

void WV::LayerStack::start()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->start();
	}
}

void WV::LayerStack::update( double _deltaTime )
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->update( _deltaTime );
	}
}

void WV::LayerStack::draw3D()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->draw3D();
	}
}

void WV::LayerStack::draw2D()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->draw2D( );
	}
}

void WV::LayerStack::drawUI()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->drawUI();
	}
}

