#include "SceneGame.h"

#include <wv/Application/Application.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/ModelParser.h>
#include <wv/Primitive/Mesh.h>

#include <glad/glad.h>
#include <SDL2/SDL_keycode.h>

DefaultScene::DefaultScene()
{
	
}

DefaultScene::~DefaultScene()
{

}

void DefaultScene::onLoad()
{
	wv::Application* app = wv::Application::get();
	
	wv::assimp::Parser parser;
	m_skybox = parser.load( "res/meshes/skysphere.dae" );
	m_skybox->transform.rotation.x = -90.0f;

	m_skyMaterial = new wv::Material();
	m_skyMaterial->loadFromFile( "sky" );

	if( m_skybox ) 
		m_skybox->primitives[ 0 ]->material = m_skyMaterial;

	subscribeInputEvent();
}

void DefaultScene::onUnload()
{
	unsubscribeInputEvent();
	
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	device->destroyMesh( &m_skybox );
	// destroy material
}

void DefaultScene::onCreate()
{
	
}

void DefaultScene::onDestroy()
{
	
}

void DefaultScene::onInputEvent( wv::InputEvent _event )
{
	if ( _event.buttondown && _event.key == SDLK_ESCAPE )
		wv::Application::get()->quit();
}

void DefaultScene::update( double _deltaTime )
{
	wv::Application* app = wv::Application::get();

}

void DefaultScene::draw( wv::GraphicsDevice* _device )
{
	/// TODO: remove raw gl calls
	glDepthMask( GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	_device->draw( m_skybox );
	glDepthFunc( GL_LESS );
	glDepthMask( GL_TRUE );
}
