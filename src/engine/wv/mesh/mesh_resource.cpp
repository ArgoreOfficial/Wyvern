#include "mesh_resource.h"

#include <wv/memory/model_parser.h>
#include <wv/engine.h>
#include <wv/graphics/graphics_device.h>
#include <wv/Resource/resource_registry.h>
#include <wv/material/material.h>

#ifdef WV_SUPPORT_OPENGL
#include <glad/glad.h>
#endif

#include <wv/shader/shader_resource.h>
#include <wv/texture/texture_resource.h>

///////////////////////////////////////////////////////////////////////////////////////

void wv::sMeshInstance::draw()
{
	if( pResource )
		pResource->addToDrawQueue( *this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::sMeshInstance::destroy()
{
	if( pResource )
		pResource->destroyInstance( *this );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMeshResource::load( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	cEngine* app = cEngine::get();

	wv::Parser parser;
	m_pMeshNode = parser.load( m_path.c_str(), app->m_pResourceRegistry );
}

///////////////////////////////////////////////////////////////////////////////////////

static void unloadMeshNode( wv::sMeshNode* _node )
{
	wv::IGraphicsDevice* pLowLevelGraphics = wv::cEngine::get()->graphics;
	wv::cResourceRegistry* pResourceRegistry = wv::cEngine::get()->m_pResourceRegistry;

	for ( auto& meshID : _node->meshes )
	{
		wv::sMesh mesh = pLowLevelGraphics->m_meshes.at( meshID );
		pResourceRegistry->unload( mesh.pMaterial );
		pLowLevelGraphics->destroyMesh( meshID );
	}
	
	for ( auto& child : _node->children )
		unloadMeshNode( child );
	
	WV_FREE( _node );
}

void wv::cMeshResource::unload( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	if( m_pMeshNode )
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
	std::scoped_lock lock{ m_mutex };

	m_drawQueue.push_back( _instance.transform );
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::cMeshResource::drawInstances( IGraphicsDevice* _pLowLevelGraphics )
{
	if ( m_pMeshNode == nullptr )
	{
		m_drawQueue.clear();
		return;
	}

	if ( m_drawQueue.empty() )
		return;

	drawNode( _pLowLevelGraphics, m_pMeshNode );

	m_drawQueue.clear();
}

void wv::cMeshResource::drawNode( IGraphicsDevice* _pLowLevelGraphics, sMeshNode* _node )
{
	if ( !_node )
		return;

	auto& meshreg = _pLowLevelGraphics->m_meshes;

	for ( auto& meshID : _node->meshes )
	{
		if ( !meshID.is_valid() )
			continue;

		sMesh mesh = meshreg.at( meshID ); // get copy, incase mesh object container reallocated
		if( !mesh.complete )
			continue;

		wv::cMaterial* mat = mesh.pMaterial;

		if ( !mat || !mat->isComplete() )
			mat = _pLowLevelGraphics->getEmptyMaterial();
		
		wv::cShaderResource* pShader = mat->getShader();
		mat->setAsActive( _pLowLevelGraphics );

		wv::GPUBufferID SbInstanceData = pShader->getShaderBuffer( "SbInstances" );
		if( SbInstanceData.is_valid() ) // TODO: enable gpu instancing on all meshes
			m_useGPUInstancing = true;
		else
			m_useGPUInstancing = false;
			
		wv::Matrix4x4f basematrix = mesh.transform.getMatrix();
		std::vector<sMeshInstanceData> instances;

		for ( auto& transform : m_drawQueue )
		{
			auto matrix = basematrix * transform.getMatrix();

			if ( !m_useGPUInstancing )
			{
				mat->setInstanceUniforms( &mesh );
				// _pLowLevelGraphics->draw( meshID );
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
						if( var.data.texture->m_textureID.is_valid() )
						{
							sTexture& tex = _pLowLevelGraphics->m_textures.at( var.data.texture->m_textureID );
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

			if ( SbVerticesID.is_valid() )
			{
				//_pLowLevelGraphics->cmdBindVertexBuffer( 0, SbVerticesID );
				{
					wv::sGPUBuffer& SbVertices = _pLowLevelGraphics->m_gpuBuffers.at( SbVerticesID );

					_pLowLevelGraphics->bindBufferIndex( mesh.vertexBufferID, SbVertices.bindingIndex.value );
					_pLowLevelGraphics->cmdBindIndexBuffer( {}, mesh.indexBufferID, 0, WV_UNSIGNED_INT );
				}

				_pLowLevelGraphics->cmdUpdateBuffer( {}, SbInstanceData, instances.size() * sizeof( sMeshInstanceData ), instances.data() );
				_pLowLevelGraphics->cmdDrawIndexed( {}, mesh.numIndices, instances.size(), mesh.baseIndex, mesh.baseVertex, 0 );
			}

			instances.clear();
		}
		
	}

	for ( auto& childNode : _node->children )
		drawNode( _pLowLevelGraphics, childNode );
}

