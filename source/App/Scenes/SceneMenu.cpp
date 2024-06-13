#include "SceneMenu.h"


#include <wv/Application/Application.h>
#include <wv/Assets/Materials/Material.h>
#include <wv/Camera/iCamera.h>
#include <wv/Debug/Print.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Device/DeviceContext.h>
#include <wv/Primitive/Primitive.h>
#include <wv/Primitive/Mesh.h>
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

}

void SceneMenu::onUnload()
{

}

void SceneMenu::onCreate()
{
	wv::Application* app = wv::Application::get();

	m_quad = new wv::Mesh();
	{
		std::vector<wv::Vertex> vertices = {
			wv::Vertex{ { 0.0f,  0.0f, 0.0f },{},{},{},{0.0f, 0.0f} },
			wv::Vertex{ { 0.0f, -1.0f, 0.0f },{},{},{},{0.0f, 1.0f} },
			wv::Vertex{ { 1.0f, -1.0f, 0.0f },{},{},{},{1.0f, 1.0f} },
			wv::Vertex{ { 1.0f,  0.0f, 0.0f },{},{},{},{1.0f, 0.0f} }
		};
		std::vector<unsigned int> indices = { 
			0, 1, 2,
			0, 2, 3
		};

		std::vector<wv::InputLayoutElement> elements = {
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
			{ 3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
			{ 4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
			{ 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
		};
		wv::InputLayout layout;
		layout.elements = elements.data();
		layout.numElements = (unsigned int)elements.size();

		wv::PrimitiveDesc prDesc;
		prDesc.type = wv::WV_PRIMITIVE_TYPE_STATIC;
		prDesc.layout = &layout;

		prDesc.vertexBuffer = vertices.data();
		prDesc.vertexBufferSize = (unsigned int)( vertices.size() * sizeof( wv::Vertex ) );
		prDesc.numVertices = (unsigned int)vertices.size();

		prDesc.indexBuffer = indices.data();
		prDesc.indexBufferSize = (unsigned int)( indices.size() * sizeof( unsigned int ) );
		prDesc.numIndices = (unsigned int)indices.size();
		
		app->device->createPrimitive( &prDesc, m_quad );
	}
	
	//addSprite( "menu/mat_menu_align", 0, 0, 640, 480 );

	addSprite( "menu/mat_menu_title",    106,  14, 429, 21 );
	addSprite( "menu/mat_menu_cubes",    130, 158, 309, 21 );
	addSprite( "menu/mat_menu_textures", 130, 206, 381, 21 );
	addSprite( "menu/mat_menu_starwars", 130, 254, 213, 21 );
	addSprite( "menu/mat_menu_credits",  130, 302, 165, 21 );
	addSprite( "menu/mat_menu_madeby",   130, 446, 333, 21 );

	m_camera = new wv::ICamera( wv::ICamera::WV_CAMERA_TYPE_ORTHOGRAPHIC );
	app->currentCamera = m_camera;

	subscribeInputEvent();
}

void SceneMenu::onDestroy()
{
	
}

void SceneMenu::onInputEvent( wv::InputEvent _event )
{
	if ( m_switching )
		return;

	if ( _event.repeat )
		return;

	if ( _event.buttondown)
	{
		switch ( _event.key )
		{
		case SDLK_DOWN: m_selected++; break;
		case SDLK_UP:   m_selected--; break;
		case SDLK_RETURN: m_switching = true; break;
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

void SceneMenu::draw()
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

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
		
		
		m_quad->transform.position = m_sprites[ i ].position - wsize;
		m_quad->transform.scale    = m_sprites[ i ].size;

		m_quad->primitives[ 0 ]->material = m_sprites[ i ].material;
		device->draw( m_quad );
	}
}

void SceneMenu::addSprite( const char* _material, int _x, int _y, int _w, int _h )
{
	MenuSprite sprite;
	sprite.material = new wv::Material();
	sprite.material->loadFromFile( _material );

	sprite.position = { (float)_x, -(float)_y, 0.0f };
	sprite.size     = { (float)_w,  (float)_h, 1.0f };

	m_sprites.push_back( sprite );
}
