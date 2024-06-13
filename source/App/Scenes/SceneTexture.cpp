#include "SceneTexture.h"


#include <wv/Application/Application.h>
#include <wv/Camera/iCamera.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/State/State.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Primitive/Sprite.h>
#include <App/Materials/AnimatedSpriteMaterial.h>


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
	spriteDesc.position = { -240.0f, 373.0f / 2.0f, 1.0f };
	spriteDesc.size     = {  480.0f, 373.0f,        1.0f };
	spriteDesc.mesh = createQuad( 21 );
	spriteDesc.material = new AnimatedSpriteMaterial();
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
	m_timer -= _deltaTime;

	if ( m_timer <= 0.0f )
	{
		m_timer = 0.04f;

		m_frame++;
		
		AnimatedSpriteMaterial* mat = (AnimatedSpriteMaterial*)m_sprite->getMaterial();
		mat->setFrame( m_frame, 21, 1 );
	}

}

void SceneTexture::draw( wv::GraphicsDevice* _device )
{
	m_sprite->draw( _device );
}

wv::Mesh* SceneTexture::createQuad( int _numFrames )
{

	wv::Application* app = wv::Application::get();

	wv::Mesh* mesh = new wv::Mesh();
	
	std::vector<wv::Vertex> vertices = {
		wv::Vertex{ { 0.0f,  0.0f, 0.0f },{},{},{},{0.0f, 0.0f} },
		wv::Vertex{ { 0.0f, -1.0f, 0.0f },{},{},{},{0.0f, 1.0f} },
		wv::Vertex{ { 1.0f, -1.0f, 0.0f },{},{},{},{1.0f / (float)_numFrames, 1.0f } },
		wv::Vertex{ { 1.0f,  0.0f, 0.0f },{},{},{},{1.0f / (float)_numFrames, 0.0f} }
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

	app->device->createPrimitive( &prDesc, mesh );
	
	return mesh;
}
