#include "GameLayer.h"
#include "FirstWindow.h"
#include "SecondWindow.h"

#include <Wyvern/Filesystem/cFilesystem.h>

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::start( void )
{

	wv::cApplication& app = wv::cApplication::getInstance();
	wv::cResourceManager& resm = wv::cResourceManager::getInstance();

	wv::Events::cKeyDownEvent::hook<GameLayer>( &GameLayer::handleKeyDownInput, this );
	wv::Events::cKeyUpEvent::hook<GameLayer>( &GameLayer::handleKeyUpInput, this );

	// m_scene.add( new FirstWindow() );
	// m_scene.add( new SecondWindow() );

	m_camera = new wv::cCamera();
	m_camera->setFOV( 45.0f );
	m_camera->setPosition( { 0.0f, 0.0f, -3.0f } );
	m_camera->setRotation( { 0.0f, 90.0f, 0.0f } );
	m_camera->update();

	m_modelHandle = resm.loadModel( "assets/meshes/logo.obj" );

	m_model = resm.getModel( m_modelHandle );
	m_model->getMaterial().loadTexture( "assets/textures/logo_fire_BaseColor.png" );

	app.getViewport().clear( wv::Color::PacificBlue );
	app.getViewport().setActiveCamera( m_camera );

} // start

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::update( double _deltaTime )
{
	wv::cApplication& app = wv::cApplication::getInstance();

	app.getViewport().clear( wv::Color::PacificBlue );
	app.getScene().update( _deltaTime );

	m_camera->move( wv::cVector3f( dx * _deltaTime * 2.0f, 0, dz * _deltaTime * 2.0f ) );
	m_camera->rotate( wv::cVector3f( 0.0f, dr * _deltaTime * 50.0f, 0.0f ) );
	m_camera->update();

} // update

void GameLayer::draw3D( void )
{
	m_model->render();
}

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::drawUI( void )
{

	wv::cApplication::getInstance().getScene().drawUI();

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
