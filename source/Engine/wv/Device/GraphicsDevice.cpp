#include "GraphicsDevice.h"

#include <wv/Debug/Print.h>
#include <wv/Debug/Trace.h>

#include <wv/Device/DeviceContext.h>

#include <wv/Primitive/Mesh.h>
#include <wv/Primitive/Primitive.h>
#include <wv/Assets/Materials/Material.h>

#ifdef WV_SUPPORT_OPENGL
#include <wv/Device/GraphicsDevice/OpenGLGraphicsDevice.h>
#endif

#ifdef WV_PLATFORM_PSVITA
#include <wv/Device/GraphicsDevice/PSVitaGraphicsDevice.h>
#endif

wv::iGraphicsDevice* wv::iGraphicsDevice::createGraphicsDevice( GraphicsDeviceDesc* _desc )
{
	
	wv::Debug::Print( Debug::WV_PRINT_DEBUG, "Creating Graphics Device\n" );

	iGraphicsDevice* device = nullptr;
#ifdef WV_PLATFORM_PSVITA
	device = new cPSVitaGraphicsDevice();
#else
#ifdef WV_SUPPORT_OPENGL
	device = new cOpenGLGraphicsDevice();
#endif
#endif

	if( !device )
		return nullptr;

	if ( !device->initialize( _desc ) )
	{
		delete device;
		return nullptr;
	}

	_desc->pContext->m_graphicsApiVersion = device->m_graphicsApiVersion;

	return device;
}

void wv::iGraphicsDevice::draw( Mesh* _mesh )
{
	WV_TRACE();

	if( !_mesh )
		return;

	_mesh->transform.update();

	for( size_t i = 0; i < _mesh->primitives.size(); i++ )
	{
		cMaterial* mat = _mesh->primitives[ i ]->material;
		if( mat )
		{
			if( !mat->isLoaded() || !mat->isCreated() )
			{
				continue;
			}

			mat->setAsActive( this );
			mat->setInstanceUniforms( _mesh );
			drawPrimitive( _mesh->primitives[ i ] );
		}
		else
		{
			drawPrimitive( _mesh->primitives[ i ] );
		}
	}
}

void wv::iGraphicsDevice::drawNode( sMeshNode* _node )
{
	WV_TRACE();

	if( !_node )
		return;

	_node->transform.update();

	for( auto& mesh : _node->meshes )
		draw( mesh );
	
	for( auto& childNode : _node->children )
		drawNode( childNode );
}
