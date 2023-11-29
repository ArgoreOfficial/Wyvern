#include "GameLayer.h"
#include "FirstWindow.h"
#include "SecondWindow.h"

#include <Wyvern/Filesystem/cFilesystem.h>
#include <Wyvern/Managers/cAssetManager.h>

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::start( void )
{

	wv::Events::cKeyDownEvent::hook<GameLayer>( &GameLayer::handleKeyDownInput, this );
	wv::Events::cKeyUpEvent::hook<GameLayer>( &GameLayer::handleKeyUpInput, this );

	// m_scene.add( new FirstWindow() );
	// m_scene.add( new SecondWindow() );

	wv::cApplication::getViewport().clear( wv::Color::PacificBlue );
	
	m_camera = new wv::cCamera();
	m_camera->setFOV( 45.0f );
	m_camera->setPosition( { 0.0f, 0.0f, -3.0f } );
	m_camera->setRotation( { 0.0f, 90.0f, 0.0f } );
	m_camera->update();

	m_model.getMaterial().loadTexture( "assets/textures/logo_fire_BaseColor.png" );
	m_model.load();

	wv::cApplication::getViewport().setActiveCamera( m_camera );

} // start

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::update( double _deltaTime )
{

	wv::cApplication::getViewport().clear( wv::Color::PacificBlue );
	wv::cApplication::getScene().update( _deltaTime );

	m_camera->move( wv::cVector3f( dx * _deltaTime * 2.0f, 0, dz * _deltaTime * 2.0f ) );
	m_camera->rotate( wv::cVector3f( 0.0f, dr * _deltaTime * 50.0f, 0.0f ) );
	m_camera->update();

} // update

void GameLayer::draw3D( void )
{
	m_model.render();
}

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::drawUI( void )
{

	wv::cApplication::getScene().drawUI();

} // drawUI

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::handleKeyDownInput( wv::Events::cKeyDownEvent& _event )
{
	// TODO: Create input manager
	if ( _event.getKeyCode() == 'W' )
		dz -= 1.0f;
	else if ( _event.getKeyCode() == 'S' )
		dz += 1.0f;
	else if ( _event.getKeyCode() == 'A' )
		dx -= 1.0f;
	else if ( _event.getKeyCode() == 'D' )
		dx += 1.0f;
	else if ( _event.getKeyCode() == GLFW_KEY_LEFT )
		dr -= 1.0f;
	else if ( _event.getKeyCode() == GLFW_KEY_RIGHT )
		dr += 1.0f;

} // handleKeyDownInput


void GameLayer::handleKeyUpInput( wv::Events::cKeyUpEvent& _event )
{
	// TODO: Create input manager
	if ( _event.getKeyCode() == 'W' )
		dz += 1.0f;
	else if ( _event.getKeyCode() == 'S' )
		dz -= 1.0f;
	else if ( _event.getKeyCode() == 'A' )
		dx += 1.0f;
	else if ( _event.getKeyCode() == 'D' )
		dx -= 1.0f;
	else if ( _event.getKeyCode() == GLFW_KEY_LEFT )
		dr += 1.0f;
	else if ( _event.getKeyCode() == GLFW_KEY_RIGHT )
		dr -= 1.0f;

} // handleKeyUpInput
