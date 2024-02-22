#include "cMesh.h"

#include <wv/Core/cRenderer.h>
#include <cm/Backends/iBackend.h>

wv::cMesh::~cMesh()
{

	if ( material )
	{
		delete material;
		material = nullptr;
	}

	cm::iBackend* backend = wv::cRenderer::getInstance().getBackend();
	backend->destroyVertexArray( vertex_array );
	backend->destroyBuffer( vertex_buffer );
	backend->destroyBuffer( index_buffer );
}
