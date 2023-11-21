#include "cLayerStack.h"

void wv::cLayerStack::start()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->start();
	}
}

void wv::cLayerStack::update( double _deltaTime )
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->update( _deltaTime );
	}
}

void wv::cLayerStack::draw3D()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->draw3D();
	}
}

void wv::cLayerStack::draw2D()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->draw2D( );
	}
}

void wv::cLayerStack::drawUI()
{
	for ( size_t i = 0; i < m_layers.size(); i++ )
	{
		m_layers[ i ]->drawUI();
	}
}

