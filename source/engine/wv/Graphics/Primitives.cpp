#include "Primitives.h"

#include <wv/Core/cApplication.h>
#include <wv/Core/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

wv::cMesh* wv::Primitives::quad( const float _size )
{
	cm::iBackend* backend = wv::cRenderer::getInstance().getBackend();

	sVertex points[] = {
		/*    Position                 Normals               Tangents                   Color              Tex Coords   */
		{ { -_size,  _size, 0 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { -_size, -_size, 0 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
		{ {  _size, -_size, 0 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
		{ {  _size,  _size, 0 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
	};

	unsigned int indices[] = {
		0,1,2,
		0,2,3
	};

	/* create vertex array */
	cm::hVertexArray vertex_array = backend->createVertexArray();
	backend->bindVertexArray( vertex_array );

	/* create vertex buffer */
	cm::sBuffer vertex_buffer = backend->createBuffer( cm::BufferType_Vertex, cm::BufferUsage_Static );
	backend->bufferData( vertex_buffer, points, sizeof( points ) );

	cm::sBuffer index_buffer = backend->createBuffer( cm::BufferType_Index, cm::BufferUsage_Static );
	backend->bufferData( index_buffer, indices, sizeof( indices ) );

	cm::cVertexLayout layout;
	layout.push<float>( 3 ); // pos
	layout.push<float>( 3 ); // normal
	layout.push<float>( 3 ); // tangent
	layout.push<float>( 4 ); // col
	layout.push<float>( 2 ); // texcoord0
	backend->bindVertexLayout( layout );

	backend->bindVertexArray( 0 );

	wv::cMesh* mesh = new wv::cMesh();

	mesh->layout = layout;
	mesh->vertex_array = vertex_array;
	mesh->vertex_buffer = vertex_buffer;
	mesh->index_buffer = index_buffer;

    return mesh; // TODO: keep track and delete
}

wv::cMesh* wv::Primitives::triangle()
{
	return nullptr;
}
