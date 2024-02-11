#include "Primitives.h"

#include <wv/cApplication.h>
#include <cm/cRenderer.h>
#include <cm/Backends/iBackend.h>

#include <wv/Math/Vector3.h>
#include <wv/Math/Vector2.h>

struct sVertex
{
	wv::cVector3f position;
	wv::cVector2f UV;
};

wv::cMesh* wv::Primitives::quad()
{
	cm::iBackend* backend = cApplication::getInstance().getRenderer()->getBackend();

	sVertex points[] = {
		/*    Position            UV      */
		{ { -0.5,  0.5, 0}, { 0.0f, 0.0f } },
		{ { -0.5, -0.5, 0}, { 0.0f, 1.0f } },
		{ {  0.5, -0.5, 0}, { 1.0f, 1.0f } },
		{ {  0.5,  0.5, 0}, { 1.0f, 0.0f } },
	};

	unsigned int indices[] = {
		0,1,2,
		0,2,3
	};

	/* mesh */

	/* create vertex array */
	cm::hVertexArray vertex_array = backend->createVertexArray();
	backend->bindVertexArray( vertex_array );

	/* create vertex buffer */
	cm::sBuffer vertex_buffer = backend->createBuffer( cm::eBufferType::BufferType_Vertex );
	backend->bufferData( vertex_buffer, points, sizeof( points ) );

	cm::sBuffer index_buffer = backend->createBuffer( cm::eBufferType::BufferType_Index );
	backend->bufferData( index_buffer, indices, sizeof( indices ) );

	cm::cVertexLayout layout;
	layout.push<float>( 3 );
	layout.push<float>( 2 );
	backend->bindVertexLayout( layout );

	backend->bindVertexArray( 0 );

	wv::cMesh* mesh = new wv::cMesh();

	mesh->layout = layout;
	mesh->vertex_array = vertex_array;
	mesh->vertex_buffer = vertex_buffer;
	mesh->index_buffer = index_buffer;

    return mesh;
}

wv::cMesh* wv::Primitives::triangle()
{

	return new wv::cMesh();
}
