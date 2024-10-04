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
	wv::CmdBufferID cmdBuffer = pGraphicsDevice->getCommandBuffer();
	wv::cResourceRegistry* pResourceRegistry = wv::cEngine::get()->m_pResourceRegistry;

	for ( auto& meshID : _node->meshes )
	{
		pGraphicsDevice->bufferCommand( cmdBuffer, wv::WV_GPUTASK_DESTROY_MESH, &meshID );
		wv::sMesh& mesh = pGraphicsDevice->m_meshes.get( meshID );
		pResourceRegistry->unload( mesh.pMaterial );
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

	bool recalcMatrices = m_pMeshNode->transform.update( nullptr );
	drawNode( _pGraphicsDevice, m_pMeshNode, recalcMatrices );

	m_drawQueue.clear();
}

void wv::cMeshResource::drawNode( iGraphicsDevice* _pGraphicsDevice, sMeshNode* _node, bool _recalcMatrices )
{
	if ( !_node )
		return;

	auto& meshreg = _pGraphicsDevice->m_meshes;

	for ( auto& meshID : _node->meshes )
	{
		if ( !meshID.isValid() )
			continue;

		sMesh& mesh = meshreg.get( meshID );
		if( !mesh.complete )
			continue;

		mesh.transform.update( &_node->transform, _recalcMatrices );

		wv::cMaterial* mat = mesh.pMaterial;

		if ( !mat || !mat->isComplete() )
			mat = _pGraphicsDevice->getEmptyMaterial();
		
		wv::cPipelineResource* pPipeline = mat->getPipeline();
		mat->setAsActive( _pGraphicsDevice );

		wv::GPUBufferID SbInstanceData = mat->getPipeline()->getShaderBuffer( "SbInstances" );
		if( SbInstanceData.isValid() ) // TODO: enable gpu instancing on all meshes
			m_useGPUInstancing = true;
		else
			m_useGPUInstancing = false;
			
		wv::cMatrix4x4f basematrix = mesh.transform.getMatrix();
		std::vector<sMeshInstanceData> instances;

		for ( auto& transform : m_drawQueue )
		{
			mesh.transform.m_matrix = basematrix * transform.getMatrix();

			if ( !m_useGPUInstancing )
			{
				mat->setInstanceUniforms( &mesh );

				wv::GPUBufferID UbInstanceData = pPipeline->getShaderBuffer( "UbInstanceData" );
				if( UbInstanceData.isValid() )
					_pGraphicsDevice->bufferData( UbInstanceData );

				_pGraphicsDevice->draw( meshID );
			}
			else
			{
				sMeshInstanceData instanceData;
				instanceData.model = mesh.transform.m_matrix;
				
				for( auto& var : mat->m_variables )
				{
					if( var.type != WV_MATERIAL_VARIABLE_TEXTURE )
						continue;

					if( var.name == "u_Albedo" )
					{
						if( var.data.texture->m_textureID.isValid() )
						{
							sTexture& tex = _pGraphicsDevice->m_textures.get( var.data.texture->m_textureID );
							instanceData.texturesHandles[ 0 ] = tex.textureHandle;
						}
					}
				}

				instances.push_back( instanceData );
			}
		}
			
		if( m_useGPUInstancing )
		{
				
			_pGraphicsDevice->bindVertexBuffer( meshID, pPipeline );

			mat->setInstanceUniforms( &mesh );
			
			wv::GPUBufferID UbInstanceData = pPipeline->getShaderBuffer( "UbInstanceData" );
			wv::GPUBufferID SbInstanceData = pPipeline->getShaderBuffer( "SbInstances" );
			if( SbInstanceData.isValid() )
			{
				wv::cGPUBuffer& SbInstanceDataBuffer = _pGraphicsDevice->m_gpuBuffers.get( SbInstanceData );
				SbInstanceDataBuffer.buffer( instances.data(), instances.size() * sizeof( sMeshInstanceData ) );
			}
	
			_pGraphicsDevice->bufferData( SbInstanceData );
			_pGraphicsDevice->bufferData( UbInstanceData );

			wv::cGPUBuffer& ibuffer = _pGraphicsDevice->m_gpuBuffers.get( mesh.indexBufferID );
			_pGraphicsDevice->drawIndexedInstances( ibuffer.count, m_drawQueue.size() );

			instances.clear();
		}
		
	}

	for ( auto& childNode : _node->children )
		drawNode( _pGraphicsDevice, childNode, _recalcMatrices );
}

