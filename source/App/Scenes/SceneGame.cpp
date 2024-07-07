#include "SceneGame.h"

#include <wv/Engine/Engine.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Memory/ModelParser.h>
#include <wv/Primitive/Mesh.h>

#include <glad/glad.h>
#include <SDL2/SDL_keycode.h>

#include <wv/Shader/ShaderRegistry.h>

///////////////////////////////////////////////////////////////////////////////////////

DefaultScene::DefaultScene()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

DefaultScene::~DefaultScene()
{

}

///////////////////////////////////////////////////////////////////////////////////////

void DefaultScene::onLoad()
{
	wv::cEngine* app = wv::cEngine::get();
	
	wv::assimp::Parser parser;
	m_skybox = parser.load( "res/meshes/skysphere.dae" );
	m_skybox->transform.rotation.x = -90.0f;

	m_mesh = parser.load( "res/meshes/debug-cube.dae" );

	m_skyMaterial = new wv::iMaterial( "sky" );
	m_skyMaterial->loadFromFile( "sky" );
	// resource leak
	if( m_skybox ) 
		m_skybox->primitives[ 0 ]->material = m_skyMaterial;

	subscribeInputEvent();
}

///////////////////////////////////////////////////////////////////////////////////////

void DefaultScene::onUnload()
{
	unsubscribeInputEvent();
	
	wv::cEngine* app = wv::cEngine::get();
	wv::iGraphicsDevice* device = app->graphics;

	device->destroyMesh( &m_skybox );
	device->destroyMesh( &m_mesh );

	m_skyMaterial->destroy( device );
}

///////////////////////////////////////////////////////////////////////////////////////

void DefaultScene::onCreate()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void DefaultScene::onDestroy()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void DefaultScene::onInputEvent( wv::InputEvent _event )
{
	if ( _event.buttondown && _event.key == SDLK_ESCAPE )
		wv::cEngine::get()->quit();
}

///////////////////////////////////////////////////////////////////////////////////////

void DefaultScene::update( double _deltaTime )
{
	wv::cEngine* app = wv::cEngine::get();
}

///////////////////////////////////////////////////////////////////////////////////////

void DefaultScene::draw( wv::iGraphicsDevice* _device )
{
	if ( m_skyMaterial->tempIsCreated() )
	{
		/// TODO: remove raw gl calls
		glDepthMask( GL_FALSE );
		glDepthFunc( GL_LEQUAL );
		_device->draw( m_skybox );
		glDepthFunc( GL_LESS );
		glDepthMask( GL_TRUE );

		_device->draw( m_mesh );
	}
}
