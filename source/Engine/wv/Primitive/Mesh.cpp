#include "Mesh.h"

#include <wv/Memory/ModelParser.h>
#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Resource/ResourceRegistry.h>
#include <wv/Material/Material.h>

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

static void unloadMeshNode( wv::sMeshNode* _node )
{
	wv::iGraphicsDevice* pGraphicsDevice = wv::cEngine::get()->graphics;
	uint32_t cmdBuffer = pGraphicsDevice->getCommandBuffer();

	for( auto& mesh : _node->meshes )
		pGraphicsDevice->bufferCommand( cmdBuffer, wv::WV_GPUTASK_DESTROY_MESH, &mesh );
	pGraphicsDevice->submitCommandBuffer( cmdBuffer );
	
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

void drawNode( wv::iGraphicsDevice* _pGraphicsDevice, wv::sMeshNode* _node, std::vector<wv::Transformf>& _transforms )
{
	if ( !_node )
		return;

	for ( auto& mesh : _node->meshes )
	{
		if ( mesh == nullptr )
			continue;

		wv::cMaterial* mat = mesh->pMaterial;
		if ( mat )
		{
			if ( !mat->isComplete() )
				continue;

			mat->setAsActive( _pGraphicsDevice );

			wv::cMatrix4x4f basematrix = mesh->transform.getMatrix();

			for ( auto& transform : _transforms )
			{
				mesh->transform.m_matrix = basematrix * transform.getMatrix();
				mat->setInstanceUniforms( mesh );

				_pGraphicsDevice->draw( mesh );
			}
		}
	}

	for ( auto& childNode : _node->children )
	{
		drawNode( _pGraphicsDevice, childNode, _transforms );
	}
}

void wv::cMeshResource::drawInstances( iGraphicsDevice* _pGraphicsDevice )
{
	if ( m_pMeshNode == nullptr )
	{
		m_drawQueue.clear();
		return;
	}

	m_pMeshNode->transform.update( nullptr );
	drawNode( _pGraphicsDevice, m_pMeshNode, m_drawQueue );

	m_drawQueue.clear();
}

