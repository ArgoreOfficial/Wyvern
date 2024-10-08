#include "MeshResource.h"

#include <wv/Memory/ModelParser.h>
#include <wv/Engine/Engine.h>
#include <wv/Graphics/Graphics.h>
#include <wv/Resource/ResourceRegistry.h>
#include <wv/Material/Material.h>

#include <glad/glad.h>
#include <wv/Shader/ShaderResource.h>
#include <wv/Texture/TextureResource.h>

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

void wv::cMeshResource::load( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics )
{
	cEngine* app = cEngine::get();

	wv::Parser parser;
	m_pMeshNode = parser.load( m_path.c_str(), app->m_pResourceRegistry );
}

///////////////////////////////////////////////////////////////////////////////////////

static void unloadMeshNode( wv::sMeshNode* _node )
{
	wv::iLowLevelGraphics* pLowLevelGraphics = wv::cEngine::get()->graphics;
	wv::CmdBufferID cmdBuffer = pLowLevelGraphics->getCommandBuffer();
	wv::cResourceRegistry* pResourceRegistry = wv::cEngine::get()->m_pResourceRegistry;

	for ( auto& meshID : _node->meshes )
	{
		pLowLevelGraphics->cmd( cmdBuffer, wv::WV_GPUTASK_DESTROY_MESH, &meshID );
		wv::sMesh& mesh = pLowLevelGraphics->m_meshes.get( meshID );
		pResourceRegistry->unload( mesh.pMaterial );
	}
	pLowLevelGraphics->submitCommandBuffer( cmdBuffer );
	
	for ( auto& child : _node->children )
		unloadMeshNode( child );
	
	delete _node;
}

void wv::cMeshResource::unload( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics )
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

void wv::cMeshResource::drawInstances( iLowLevelGraphics* _pLowLevelGraphics )
{
	if ( m_pMeshNode == nullptr )
	{
		m_drawQueue.clear();
		return;
	}

	if ( m_drawQueue.empty() )
		return;

	bool recalcMatrices = m_pMeshNode->transform.update( nullptr );
	drawNode( _pLowLevelGraphics, m_pMeshNode, recalcMatrices );

	m_drawQueue.clear();
}

void wv::cMeshResource::drawNode( iLowLevelGraphics* _pLowLevelGraphics, sMeshNode* _node, bool _recalcMatrices )
{
	if ( !_node )
		return;

	auto& meshreg = _pLowLevelGraphics->m_meshes;

	for ( auto& meshID : _node->meshes )
	{
		if ( !meshID.isValid() )
			continue;

		sMesh& mesh = meshreg.get( meshID );
		if( !mesh.complete )
			continue;

		wv::cMaterial* mat = mesh.pMaterial;

		if ( !mat || !mat->isComplete() )
			mat = _pLowLevelGraphics->getEmptyMaterial();
		
		wv::cShaderResource* pShader = mat->getShader();
		mat->setAsActive( _pLowLevelGraphics );

		wv::GPUBufferID SbInstanceData = pShader->getShaderBuffer( "SbInstances" );
		if( SbInstanceData.isValid() ) // TODO: enable gpu instancing on all meshes
			m_useGPUInstancing = true;
		else
			m_useGPUInstancing = false;
			
		wv::cMatrix4x4f basematrix = mesh.transform.getMatrix();
		std::vector<sMeshInstanceData> instances;

		for ( auto& transform : m_drawQueue )
		{
			auto matrix = basematrix * transform.getMatrix();

			if ( !m_useGPUInstancing )
			{
				mat->setInstanceUniforms( &mesh );
				_pLowLevelGraphics->draw( meshID );
			}
			else
			{
				sMeshInstanceData instanceData;
				instanceData.model = matrix;
				
				for( auto& var : mat->m_variables )
				{
					if( var.type != WV_MATERIAL_VARIABLE_TEXTURE )
						continue;

					if( var.name == "u_Albedo" )
					{
						if( var.data.texture->m_textureID.isValid() )
						{
							sTexture& tex = _pLowLevelGraphics->m_textures.get( var.data.texture->m_textureID );
							instanceData.texturesHandles[ 0 ] = tex.textureHandle;
							instanceData.hasAlpha = tex.numChannels == 4;
						}
					}
				}

				instances.push_back( instanceData );
			}
		}
			
		if( m_useGPUInstancing )
		{
			wv::GPUBufferID SbVerticesID = pShader->getShaderBuffer( "SbVertices" );

			if( SbVerticesID.isValid() )
				_pLowLevelGraphics->bindVertexBuffer( mesh.indexBufferID, SbVerticesID );
			
			mat->setInstanceUniforms( &mesh );
			_pLowLevelGraphics->bufferData( SbInstanceData, instances.data(), instances.size() * sizeof( sMeshInstanceData ) );
			
			wv::sGPUBuffer& ibuffer = _pLowLevelGraphics->m_gpuBuffers.get( mesh.indexBufferID );
			_pLowLevelGraphics->drawIndexedInstanced( ibuffer.count, m_drawQueue.size(), mesh.baseVertex );

			instances.clear();
		}
		
	}

	for ( auto& childNode : _node->children )
		drawNode( _pLowLevelGraphics, childNode, _recalcMatrices );
}

