#include "SceneTexture.h"


#include <wv/Application/Application.h>
#include <wv/Camera/iCamera.h>
#include <wv/State/State.h>

#include <wv/Primitive/Sprite.h>
#include <wv/Assets/Materials/Material.h>

#include <SDL2/SDL_keycode.h>

SceneTexture::SceneTexture()
{

}

SceneTexture::~SceneTexture()
{

}

void SceneTexture::onLoad()
{
	subscribeInputEvent();
}

void SceneTexture::onUnload()
{
	unsubscribeInputEvent();
}

void SceneTexture::onCreate()
{
	wv::Application* app = wv::Application::get();

	m_camera = new wv::ICamera( wv::ICamera::WV_CAMERA_TYPE_ORTHOGRAPHIC );
	app->currentCamera = m_camera;

	wv::SpriteDesc spriteDesc;
	spriteDesc.material = new wv::Material();
	spriteDesc.material->loadFromFile( "animated_textures/mat_animated_textures" );

	m_sprite = wv::Sprite::create( &spriteDesc );
}

void SceneTexture::onDestroy()
{
	
}

void SceneTexture::onInputEvent( wv::InputEvent _event )
{
	if( _event.buttondown && _event.key == SDLK_ESCAPE )
		wv::Application::get()->m_applicationState->switchToScene( "SceneMenu" );
}

void SceneTexture::update( double _deltaTime )
{
	
}

void SceneTexture::draw( wv::GraphicsDevice* _device )
{
	m_sprite->draw( _device );
}
