#include "GameLayer.h"
#include "FirstWindow.h"
#include "SecondWindow.h"

#include <Wyvern/Filesystem/cFilesystem.h>

#include <Wyvern/Renderer/Camera/cCamera2D.h>
#include <Wyvern/Renderer/Camera/cCamera3D.h>

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::start( void )
{

	wv::cApplication& app = wv::cApplication::getInstance();
	wv::cResourceManager& resm = wv::cResourceManager::getInstance();

	wv::Events::cKeyDownEvent::hook<GameLayer>( &GameLayer::handleKeyDownInput, this );
	wv::Events::cKeyUpEvent::hook<GameLayer>( &GameLayer::handleKeyUpInput, this );

	// m_scene.add( new FirstWindow() );
	// m_scene.add( new SecondWindow() );

	m_camera3D = new wv::cCamera3D();
	m_camera2D = new wv::cCamera2D();

	m_camera3D->setFOV( 45.0f );
	m_camera3D->setPosition( { 0.0f, 0.0f, 3.0f } );
	
	m_modelHandle = resm.loadModel( "assets/meshes/logo.obj" );

	m_model = resm.getModel( m_modelHandle );
	m_model->getMaterial().loadTexture( "assets/textures/logo_fire_BaseColor.png" );

	app.getViewport().clear( wv::Color::PacificBlue );
	app.getViewport().setActiveCamera( m_camera2D );

} // start

///////////////////////////////////////////////////////////////////////////////////////

void GameLayer::update( double _deltaTime )
{
	wv::cApplication& app = wv::cApplication::getInstance();

	app.getViewport().clear( wv::Color::PacificBlue );
	app.getScene().update( _deltaTime );

	m_camera3D->move( wv::cVector3f( dx * _deltaTime * 2.0f, 0, dz * _deltaTime * 2.0f ) );
	m_camera3D->rotate( wv::cVector3f( 0.0f, dr * _deltaTime * 50.0f, 0.0f ) );

} // update

void GameLayer::draw3D( void )
{

	wv::cApplication::getInstance().getViewport().setActiveCamera( m_camera3D );

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
