#include "graphics_device.h"

#include <wv/debug/log.h>
#include <wv/debug/trace.h>

#include <wv/material/material.h>
#include <wv/device/device_context.h>
#include <wv/graphics/mesh.h>
#include <wv/graphics/render_target.h>

#include <wv/engine.h>
#include <wv/resource/resource_registry.h>

#include <noapi/noapi_graphics_device.h>
#if defined( WV_SUPPORT_OPENGL )
#include <opengl/opengl_graphics_device.h>
#elif defined( WV_PLATFORM_PSVITA )
#include <psp2/psp2_graphics_device.h>
#endif

#include <exception>

///////////////////////////////////////////////////////////////////////////////////////

wv::IGraphicsDevice* wv::IGraphicsDevice::createGraphics( LowLevelGraphicsDesc* _desc )
{

	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Graphics Device\n" );

	IGraphicsDevice* device = nullptr;
	switch( _desc->pContext->getGraphicsAPI() )
	{

	case GraphicsAPI::WV_GRAPHICS_API_NONE:   
		device = WV_NEW( NoAPIGraphicsDevice );
		break;

#ifdef WV_SUPPORT_OPENGL
	case GraphicsAPI::WV_GRAPHICS_API_OPENGL: 
		device = WV_NEW( GraphicsDeviceOpenGL );
		break;
#endif

#ifdef WV_PLATFORM_PSVITA
	case GraphicsAPI::WV_GRAPHICS_API_PSVITA: 
		device = WV_NEW( cPSVitaGraphicsDevice );
		break;
#endif
	}

	if( !device )
		return nullptr;

	if( !device->initialize( _desc ) )
	{
		WV_FREE( device );
		return nullptr;
	}

	device->m_threadID = std::this_thread::get_id();
	_desc->pContext->m_graphicsApiVersion = device->m_graphicsApiVersion;
	
	return device;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::initEmbeds()
{
	m_pEmptyMaterial = WV_NEW( Material, "empty", "materials/EmptyMaterial.wmat" );
	m_pEmptyMaterial->load( Engine::get()->m_pFileSystem, Engine::get()->graphics );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::terminate()
{
	m_pEmptyMaterial->unload( Engine::get()->m_pFileSystem, Engine::get()->graphics );
	WV_FREE( m_pEmptyMaterial );
	m_pEmptyMaterial = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::beginRender()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::IGraphicsDevice::endRender()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

wv::IGraphicsDevice::IGraphicsDevice()
{

}

wv::MeshID wv::IGraphicsDevice::createMesh( const MeshDesc& _desc )
{
	MeshID id = m_meshes.emplace();

	Mesh mesh{};
	mesh.pMaterial   = _desc.pMaterial;
	mesh.numVertices = _desc.sizeVertices / sizeof( Vertex ); 
	
	{ // create index and vertex buffers
		GPUBufferDesc vbDesc{};
		vbDesc.name = "vertexBuffer";
		vbDesc.type = WV_BUFFER_TYPE_DYNAMIC;
		vbDesc.usage = WV_BUFFER_USAGE_DYNAMIC_DRAW;
		vbDesc.size = _desc.sizeVertices;
		mesh.vertexBufferID = createGPUBuffer( vbDesc );

		bufferSubData( mesh.vertexBufferID, _desc.vertices, _desc.sizeVertices, 0 );
	}

	if( _desc.numIndices > 0 )
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_INDICES;

		GPUBufferDesc ibDesc;
		ibDesc.name = "ebo";
		ibDesc.type = WV_BUFFER_TYPE_INDEX;
		ibDesc.usage = WV_BUFFER_USAGE_STATIC_DRAW;

		void* indices = nullptr;
		size_t sizeIndices = 0;

		if( _desc.pIndices16 )
		{
			sizeIndices = _desc.numIndices * sizeof( uint16_t );
			indices = _desc.pIndices16;
		}
		else if( _desc.pIndices32 )
		{
			sizeIndices = _desc.numIndices * sizeof( uint32_t );
			indices = _desc.pIndices32;
		}

		ibDesc.size = sizeIndices;

		mesh.indexBufferID = createGPUBuffer( ibDesc );
		mesh.numIndices = _desc.numIndices;

		bufferSubData( mesh.indexBufferID, indices, sizeIndices, 0 );
	}
	else
	{
		mesh.drawType = WV_MESH_DRAW_TYPE_VERTICES;
	}

	if( _desc.deleteData )
	{
		if( _desc.vertices )   { WV_FREE_ARR( _desc.vertices ); }
		if( _desc.pIndices16 ) { WV_FREE_ARR( _desc.pIndices16 ); }
		if( _desc.pIndices32 ) { WV_FREE_ARR( _desc.pIndices32 ); }
	}

	// mesh.transform.update( _desc.pParentTransform );
	if( _desc.pParentTransform )
		mesh.transform.m_matrix = _desc.pParentTransform->m_matrix;

	mesh.complete = true;
	m_meshes.at( id ) = mesh;

	return id;
}

void wv::IGraphicsDevice::destroyMesh( MeshID _meshID )
{
	m_meshes.erase( _meshID );
}
