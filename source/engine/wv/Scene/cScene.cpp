#include "cScene.h"

#include <wv/Scene/iSceneObject.h>

void wv::cScene::create( void )
{
}

void wv::cScene::destroy( void )
{
}

void wv::cScene::load( void )
{
}

void wv::cScene::unload( void )
{
}

void wv::cScene::update( double _delta_time )
{
	for ( int i = 0; i < (int)m_objects.size(); i++ )
		m_objects[ i ]->update( _delta_time );
	
}

void wv::cScene::render( void )
{
	for ( int i = 0; i < (int)m_objects.size(); i++ )
		m_objects[ i ]->render();

}

void wv::cScene::addObject( iSceneObject* _object )
{
	m_objects.push_back( _object );
}
