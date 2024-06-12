#include "SceneMenu.h"


#include <wv/Application/Application.h>
#include <wv/Camera/iCamera.h>
#include <wv/Debug/Print.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Device/DeviceContext.h>

#include <wv/Assets/Materials/Material.h>

#include <wv/Primitive/Primitive.h>
#include <wv/Primitive/Mesh.h>


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
	
	addSprite( "menu/mat_menu_align", 0, 0, 640, 480 );

	m_camera = new wv::ICamera( wv::ICamera::WV_CAMERA_TYPE_ORTHOGRAPHIC );
	app->currentCamera = m_camera;
}

void SceneMenu::onDestroy()
{
	
}

void SceneMenu::onInputEvent( wv::InputEvent _event )
{

}

void SceneMenu::update( double _deltaTime )
{
	
}

void SceneMenu::draw()
{
	wv::Application* app = wv::Application::get();
	wv::GraphicsDevice* device = app->device;

	const wv::Vector3f wsize{ 640 / 2, -480 / 2, 0 };
	for ( int i = 0; i < (int)m_sprites.size(); i++ )
	{
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

	sprite.position = { (float)_x, (float)_y, 0.0f };
	sprite.size     = { (float)_w, (float)_h, 1.0f };

	m_sprites.push_back( sprite );
}
