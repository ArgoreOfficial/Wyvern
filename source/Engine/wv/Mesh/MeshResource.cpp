#include "MeshResource.h"

#include <wv/Memory/ModelParser.h>
#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>
#include <wv/Resource/ResourceRegistry.h>
#include <wv/Material/Material.h>

#include <glad/glad.h>
#include <wv/Shader/ShaderProgram.h>
#include <wv/Texture/Texture.h>

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
		if ( !mat || !mat->isComplete() )
			mat = _pGraphicsDevice->getEmptyMaterial();
		
		wv::cProgramPipeline* pPipeline = mat->getPipeline();
		mat->setAsActive( _pGraphicsDevice );

		wv::cGPUBuffer* SbInstanceData = mat->getPipeline()->getShaderBuffer( "SbInstances" );
		if( SbInstanceData ) // TODO: enable gpu instancing on all meshes
			m_useGPUInstancing = true;
		else
			m_useGPUInstancing = false;
			
		wv::cMatrix4x4f basematrix = mesh->transform.getMatrix();
		std::vector<sMeshInstanceData> instances;

		for ( auto& transform : m_drawQueue )
		{
			mesh->transform.m_matrix = basematrix * transform.getMatrix();

			if ( !m_useGPUInstancing )
			{
				mat->setInstanceUniforms( mesh );

				wv::cGPUBuffer* UbInstanceData = pPipeline->getShaderBuffer( "UbInstanceData" );
				if( UbInstanceData )
					_pGraphicsDevice->bufferData( UbInstanceData );

				_pGraphicsDevice->draw( mesh );
			}
			else
			{
				sMeshInstanceData instanceData;
				instanceData.model = mesh->transform.m_matrix;
				
				for( auto& var : mat->m_variables )
				{
					if( var.type != WV_MATERIAL_VARIABLE_TEXTURE )
						continue;

					if( var.name == "u_Albedo" )
						instanceData.texturesHandles[ 0 ] = var.data.texture->m_texture.textureHandle;
				}

				instances.push_back( instanceData );
			}
		}
			
		if( m_useGPUInstancing )
		{
				
			_pGraphicsDevice->bindVertexBuffer( mesh, pPipeline );

			mat->setInstanceUniforms( mesh );
			
			wv::cGPUBuffer* UbInstanceData = pPipeline->getShaderBuffer( "UbInstanceData" );
			wv::cGPUBuffer* SbInstanceData = pPipeline->getShaderBuffer( "SbInstances" );
			if ( SbInstanceData )
				SbInstanceData->buffer( instances.data(), instances.size() * sizeof( sMeshInstanceData ) );
	
			_pGraphicsDevice->bufferData( SbInstanceData );
			_pGraphicsDevice->bufferData( UbInstanceData );

			_pGraphicsDevice->drawIndexedInstances( mesh->pIndexBuffer->count, m_drawQueue.size() );

			instances.clear();
		}
		
	}

	for ( auto& childNode : _node->children )
		drawNode( _pGraphicsDevice, childNode );
}

