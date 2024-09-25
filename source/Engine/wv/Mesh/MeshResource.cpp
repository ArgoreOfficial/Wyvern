#include "MeshResource.h"

#include <wv/Memory/ModelParser.h>
#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Resource/ResourceRegistry.h>
#include <wv/Material/Material.h>

#include <glad/glad.h>
#include <wv/Shader/ShaderProgram.h>


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
	wv::cResourceRegistry* pResourceRegistry = wv::cEngine::get()->m_pResourceRegistry;

	for ( auto& mesh : _node->meshes )
	{
		pGraphicsDevice->bufferCommand( cmdBuffer, wv::WV_GPUTASK_DESTROY_MESH, &mesh );
		pResourceRegistry->unload( mesh->pMaterial );
	}
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

void wv::cMeshResource::drawInstances( iGraphicsDevice* _pGraphicsDevice )
{
	if ( m_pMeshNode == nullptr )
	{
		m_drawQueue.clear();
		return;
	}

	if ( m_drawQueue.empty() )
		return;

	m_pMeshNode->transform.update( nullptr );
	drawNode( _pGraphicsDevice, m_pMeshNode );

	m_drawQueue.clear();
}

void wv::cMeshResource::drawNode( iGraphicsDevice* _pGraphicsDevice, sMeshNode* _node )
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

			wv::cGPUBuffer* SbInstanceData = mat->getPipeline()->getShaderBuffer( "SbInstances" );
			if( SbInstanceData ) // TODO: enable gpu instancing on all meshes
				m_useGPUInstancing = true;
			else
				m_useGPUInstancing = false;
			
			wv::cMatrix4x4f basematrix = mesh->transform.getMatrix();
			std::vector<cMatrix4x4f> matrices;

			for ( auto& transform : m_drawQueue )
			{
				mesh->transform.m_matrix = basematrix * transform.getMatrix();

				if ( !m_useGPUInstancing )
				{
					mat->setInstanceUniforms( mesh );
					_pGraphicsDevice->draw( mesh );
				}
				else
					matrices.push_back( mesh->transform.m_matrix );
			}
			
			if( m_useGPUInstancing )
			{
				
				_pGraphicsDevice->bindVertexArray( mesh );

				mat->setInstanceUniforms( mesh );

				wv::cGPUBuffer* SbInstanceData = mat->getPipeline()->getShaderBuffer( "SbInstances" );
				if ( SbInstanceData )
					SbInstanceData->buffer( matrices.data(), matrices.size() * sizeof( cMatrix4x4f ) );

				std::vector<cGPUBuffer*>& shaderBuffers = mat->getPipeline()->m_pPipeline->pVertexProgram->shaderBuffers;
				for ( auto& buf : shaderBuffers )
					_pGraphicsDevice->bufferData( buf );

				_pGraphicsDevice->drawIndexedInstances( mesh->pIndexBuffer->count, m_drawQueue.size() );

				matrices.clear();
			}
		}
	}

	for ( auto& childNode : _node->children )
		drawNode( _pGraphicsDevice, childNode );
}

