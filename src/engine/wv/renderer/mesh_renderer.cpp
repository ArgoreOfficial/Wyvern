#include "mesh_renderer.h"

#include <wv/mesh/mesh_resource.h>
#include <wv/material/material.h>
#include <wv/texture/texture_resource.h>
#include <wv/graphics/graphics_device.h>
#include <wv/shader/shader_resource.h>
#include <wv/graphics/mesh.h>

void wv::IMeshRenderer::drawMeshNode( MeshNode* _pNode, Matrix4x4f* _pInstanceMatrices, size_t _numInstances )
{
	if ( !_pNode )
		return;

	unordered_array<MeshID, Mesh>& meshreg = m_pGraphics->m_meshes;

	for ( auto& meshID : _pNode->meshes )
	{
		if ( !meshID.is_valid() )
			continue;

		Mesh mesh = meshreg.at( meshID ); // get copy, incase mesh object container reallocated
		if ( !mesh.complete )
			continue;

		wv::Material* mat = mesh.pMaterial;

		if ( !mat || !mat->isComplete() )
			mat = m_pGraphics->getEmptyMaterial();

		wv::ShaderResource* pShader = mat->getShader();
		mat->setAsActive( m_pGraphics );

		wv::GPUBufferID SbInstanceData = pShader->getShaderBuffer( "SbInstances" );
		
		wv::Matrix4x4f basematrix = mesh.transform.getMatrix();
		std::vector<MeshInstanceData> instances;
		
		for ( size_t i = 0; i < _numInstances; i++ )
		{
			Matrix4x4f matrix = basematrix * _pInstanceMatrices[ i ];
		
			MeshInstanceData instanceData{};
			instanceData.model = matrix;

			for ( auto& var : mat->m_variables )
			{
				if ( var.type != WV_MATERIAL_VARIABLE_TEXTURE )
					continue;

				if ( var.name == "u_Albedo" )
				{
					if ( var.data.texture->m_textureID.is_valid() )
					{
						Texture& tex = m_pGraphics->m_textures.at( var.data.texture->m_textureID );
						instanceData.texturesHandles[ 0 ] = tex.textureHandle;
						instanceData.hasAlpha = tex.numChannels == 4;
					}
				}
			}

			instances.push_back( instanceData );
		}

		wv::GPUBufferID SbVerticesID = pShader->getShaderBuffer( "SbVertices" );

		if ( SbVerticesID.is_valid() )
		{
			wv::GPUBuffer& SbVertices = m_pGraphics->m_gpuBuffers.at( SbVerticesID );

			m_pGraphics->bindBufferIndex( mesh.vertexBufferID, SbVertices.bindingIndex.value );
			m_pGraphics->cmdBindIndexBuffer( {}, mesh.indexBufferID, 0, WV_UNSIGNED_INT );
			m_pGraphics->cmdUpdateBuffer( {}, SbInstanceData, instances.size() * sizeof( MeshInstanceData ), instances.data() );
			m_pGraphics->cmdDrawIndexed( {}, mesh.numIndices, instances.size(), mesh.baseIndex, mesh.baseVertex, 0 );
		}

		instances.clear();
	}

	for ( auto& childNode : _pNode->children )
		drawMeshNode( childNode, _pInstanceMatrices, _numInstances );
}

void wv::IMeshRenderer::drawMesh( MeshResource* _pMesh )
{
	auto& instances = _pMesh->getInstances();
	if ( _pMesh->getMeshNode() == nullptr )
	{
		instances.clear();
		return;
	}

	if ( instances.empty() )
		return;

	// construct instance list
	std::vector<Matrix4x4f> matrices;
	matrices.reserve( instances.size() );
	for ( MeshInstance* instance : instances )
		matrices.push_back( instance->transform.getMatrix() );
	
	drawMeshNode( _pMesh->getMeshNode(), matrices.data(), matrices.size() );
}
