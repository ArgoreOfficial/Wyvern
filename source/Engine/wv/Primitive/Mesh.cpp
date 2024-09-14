#include "Mesh.h"

#include <wv/Memory/ModelParser.h>
#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Resource/ResourceRegistry.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::sMeshInstance::draw()
{
	pResource->addToDrawQueue( *this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::sMeshInstance::destroy()
{
	pResource->destroyInstance( *this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMeshResource::load( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{
	cEngine* app = cEngine::get();

	wv::Parser parser;
	m_pMeshNode = parser.load( m_path.c_str(), app->m_pResourceRegistry );
}

///////////////////////////////////////////////////////////////////////////////////////

static void unloadMesh( wv::sMesh* _mesh )
{
	wv::iGraphicsDevice* pGraphicsDevice = wv::cEngine::get()->graphics;
	wv::cCommandBuffer& cmdBuffer = pGraphicsDevice->getCommandBuffer();
	
	for ( auto& primitive : _mesh->primitives )
		cmdBuffer.push( wv::WV_GPUTASK_DESTROY_PRIMITIVE, &primitive );
	
	cmdBuffer.callback.m_fptr = []( void* _c )
		{
			wv::sMesh* mesh = (wv::sMesh*)_c;
			delete mesh;
		};

	cmdBuffer.callbacker = _mesh;

	pGraphicsDevice->submitCommandBuffer( cmdBuffer );
}

static void unloadMeshNode( wv::sMeshNode* _node )
{
	for ( auto& mesh : _node->meshes )
		unloadMesh( mesh );
	
	for ( auto& child : _node->children )
		unloadMeshNode( child );
	
	delete _node;
}

void wv::cMeshResource::unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{
	unloadMeshNode( m_pMeshNode );
}

///////////////////////////////////////////////////////////////////////////////////////

wv::sMeshInstance wv::cMeshResource::createInstance()
{
	sMeshInstance instance;
	instance.pResource = this;
	
    return instance;
}

void wv::cMeshResource::destroyInstance( sMeshInstance& _instance )
{
	cEngine* app = cEngine::get();
	app->m_pResourceRegistry->unload( _instance.pResource );

	_instance.pResource = nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMeshResource::addToDrawQueue( sMeshInstance& _instance )
{
	m_drawQueue.push_back( _instance.transform );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMeshResource::drawInstances( iGraphicsDevice* _pGraphicsDevice )
{
	if ( m_pMeshNode == nullptr )
	{
		m_drawQueue.clear();
		return;
	}

	for ( auto& transform : m_drawQueue )
	{
		m_pMeshNode->transform.update( &transform );
		_pGraphicsDevice->drawNode( m_pMeshNode );
	}

	m_drawQueue.clear();
}

