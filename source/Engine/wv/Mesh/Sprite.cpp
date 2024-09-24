#include "Sprite.h"

#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Mesh/MeshResource.h>
#include <wv/Material/Material.h>

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
	/*
	sMeshWrapper* m = m_mesh ? m_mesh : Internal::S_SPRITE_QUAD;
	m->transform = m_transform;
	m->primitives[ 0 ]->material = m_material;
	_device->draw( m );
	*/
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Sprite::createQuad()
{
	wv::cEngine* app = wv::cEngine::get();

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

	std::vector<wv::sVertexAttribute> elements = {
		{ "a_Pos",       3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
		{ "a_Normal",    3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
		{ "a_Tangent",   3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
		{ "a_Color",     4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
		{ "a_TexCoord0", 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
	};
	wv::sVertexLayout layout;
	layout.elements = elements.data();
	layout.numElements = (unsigned int)elements.size();

	wv::sMeshDesc prDesc;
	prDesc.layout = layout;

	prDesc.vertices     = vertices.data();
	prDesc.sizeVertices = vertices.size() * sizeof( Vertex );

	prDesc.pIndices32  = indices.data();
	prDesc.numIndices = indices.size();

	Internal::S_SPRITE_QUAD = app->graphics->createMesh( &prDesc );
	
}
