#include "Sprite.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Primitive/Mesh.h>
#include <wv/Assets/Materials/Material.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::Sprite::~Sprite()
{

}

///////////////////////////////////////////////////////////////////////////////////////

wv::Sprite* wv::Sprite::create( SpriteDesc* _desc )
{
	Sprite* sprite = new Sprite();
	sprite->m_material = _desc->material;
	sprite->m_transform.setPosition( _desc->position );
	sprite->m_transform.setScale( _desc->size );
	sprite->m_mesh = _desc->mesh;

	if ( !Internal::S_SPRITE_QUAD )
		createQuad();
	
    return sprite;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Sprite::draw( iGraphicsDevice* _device )
{
	if ( !Internal::S_SPRITE_QUAD )
		return;
	
	Mesh* m = m_mesh ? m_mesh : Internal::S_SPRITE_QUAD;
	m->transform = m_transform;
	m->primitives[ 0 ]->material = m_material;
	_device->draw( m );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Sprite::createQuad()
{
	wv::cEngine* app = wv::cEngine::get();

	Internal::S_SPRITE_QUAD = new wv::Mesh();
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

		app->graphics->createPrimitive( &prDesc, Internal::S_SPRITE_QUAD );
	}
}
