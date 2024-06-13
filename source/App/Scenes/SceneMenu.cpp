#include "SceneMenu.h"


#include <wv/Application/Application.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Camera/iCamera.h>
#include <wv/Debug/Print.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Primitive/Primitive.h>
#include <wv/Primitive/Sprite.h>
#include <wv/State/State.h>

#include <SDL2/SDL_keycode.h>


SceneMenu::SceneMenu()
{

}

SceneMenu::~SceneMenu()
{

}

void SceneMenu::onLoad()
{
	subscribeInputEvent();

	wv::Application* app = wv::Application::get();

	app->context->setMouseLock( false );
	app->currentCamera = m_camera;
}

void SceneMenu::onUnload()
{
	unsubscribeInputEvent();
}

void SceneMenu::onCreate()
{
	wv::Application* app = wv::Application::get();

	addSprite( "menu/mat_menu_title",    106,  14, 429, 21 );
	addSprite( "menu/mat_menu_cubes",    130, 158, 309, 21 );
	addSprite( "menu/mat_menu_textures", 130, 206, 381, 21 );
	addSprite( "menu/mat_menu_starwars", 130, 254, 213, 21 );
	addSprite( "menu/mat_menu_credits",  130, 302, 165, 21 );
	addSprite( "menu/mat_menu_madeby",   130, 446, 333, 21 );

	m_camera = new wv::ICamera( wv::ICamera::WV_CAMERA_TYPE_ORTHOGRAPHIC );
}

void SceneMenu::onDestroy()
{
	
}

void SceneMenu::onInputEvent( wv::InputEvent _event )
{
	if ( m_switching || _event.repeat )
		return;

	if ( _event.buttondown)
	{
		switch ( _event.key )
		{
		case SDLK_DOWN: m_selected++; break;
		case SDLK_UP:   m_selected--; break;
		case SDLK_RETURN: m_switching = true; break;

		case SDLK_ESCAPE: wv::Application::get()->quit(); break;
		}
		m_selected = wv::Math::clamp( m_selected, 0, 3 );
	}
}

void SceneMenu::update( double _deltaTime )
{
	m_blinkerTimer -= _deltaTime;
	if ( m_blinkerTimer <= 0.0f )
	{
		m_blinker = !m_blinker;
		m_blinkerTimer = 0.6f;
	}

	if ( m_switching )
	{
		if ( m_switchCooldown <= 0 )
		{
			switch ( m_selected )
			{
			case 2: wv::Application::get()->m_applicationState->switchToScene( "SceneGame" ); break;
			}
			m_switching = false;
			m_switchCooldown = 1;
		}
		else
			m_switchCooldown--;
	}
}

void SceneMenu::draw( wv::GraphicsDevice* _device )
{
	const wv::Vector3f wsize{ 640 / 2, -480 / 2, 0 };
	for ( int i = 0; i < (int)m_sprites.size(); i++ )
	{
		if ( m_switching )
		{
			if( i != 0 && i != 5 && i != m_selected + 1 )
				continue;
		}
		else if ( i == m_selected + 1 && !m_blinker )
			continue;
		
		
		m_sprites[ i ]->draw( _device );
	}
}

void SceneMenu::addSprite( const char* _material, int _x, int _y, int _w, int _h )
{
	wv::SpriteDesc desc;
	
	desc.material = new wv::Material();
	desc.material->loadFromFile( _material );

	desc.position = { (float)_x, -(float)_y, 0.0f };
	desc.size     = { (float)_w,  (float)_h, 1.0f };

	m_sprites.push_back( wv::Sprite::create( &desc ) );
}
