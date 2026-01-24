#include "mesh_asset_loader.h"

#include <wv/filesystem/asset_manager.h>

#include <wv/debug/log.h>
#include <wv/filesystem/file_system.h>

#include <string>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

template <>
struct fastgltf::ElementTraits<wv::Vector3f> : fastgltf::ElementTraitsBase<wv::Vector3f, AccessorType::Vec3, float> { };

template <>
struct fastgltf::ElementTraits<wv::Vector2f> : fastgltf::ElementTraitsBase<wv::Vector2f, AccessorType::Vec2, float> { };

wv::ResourceID wv::MeshAssetLoader::load( const std::filesystem::path& _path )
{
	ResourceID res{};
	MeshAsset meshAsset{};
	meshAsset.path = _path;

	if ( m_assetManager->containsAsset( _path ) )
	{
		res = m_assetManager->getAsset( _path );
	}
	else
	{
		// Load and parse file 

		fastgltf::Parser parser;
		std::filesystem::path fullpath = m_filesystem->getFullPath( _path );
		
		fastgltf::Expected<fastgltf::GltfDataBuffer> data = fastgltf::GltfDataBuffer::FromPath( fullpath );
		if ( data.error() != fastgltf::Error::None )
		{
			WV_LOG_ERROR( "Failed to load model %s\n", _path.string().c_str() );
			return {};
		}

		fastgltf::Expected<fastgltf::Asset> load = parser.loadGltf( data.get(), fullpath.parent_path(), fastgltf::Options::LoadExternalBuffers );
		if ( load.error() != fastgltf::Error::None )
		{
			WV_LOG_ERROR( "Failed to parse model%s\n", _path.string().c_str() );
			return {};
		}

		// Create mesh asset

		fastgltf::Asset& asset = load.get();
			
		for ( fastgltf::Mesh& mesh : asset.meshes )
		{
			for ( auto& primitive : mesh.primitives )
			{
				std::vector<uint16_t> indices;
				std::vector<Vector3f> positions;
				std::vector<Vector3f> normals;
				std::vector<Vector2f> uv0s;
				std::vector<Vector3f> colours;

				GeometrySurface surface;
				surface.startIndex = meshAsset.indices.size();
				
				fastgltf::Attribute* posAttrib = primitive.findAttribute( "POSITION" );
				fastgltf::Attribute* norAttrib = primitive.findAttribute( "NORMAL" );
				fastgltf::Attribute* uv0Attrib = primitive.findAttribute( "TEXCOORD_0" );
				fastgltf::Attribute* colAttrib = primitive.findAttribute( "COLOR_0" );

				// load indices
				{
					fastgltf::Accessor& indexAccessor = asset.accessors[ primitive.indicesAccessor.value() ];
					surface.indexCount = indexAccessor.count;
					indices.resize( indexAccessor.count );
					fastgltf::copyFromAccessor<uint16_t>( asset, indexAccessor, indices.data() );
				}

				// load vertex data
				{
					// vertex position
					if ( posAttrib != primitive.attributes.end() )
					{
						fastgltf::Accessor& positionAccessor = asset.accessors[ posAttrib->accessorIndex ];
						surface.vertexCount = positionAccessor.count;
						positions.resize( positionAccessor.count );
						fastgltf::copyFromAccessor<Vector3f>( asset, positionAccessor, positions.data() );
					}
					
					// vertex normal
					if ( norAttrib != primitive.attributes.end() )
					{
						fastgltf::Accessor& normalAccessor = asset.accessors[ norAttrib->accessorIndex ];
						normals.resize( normalAccessor.count );
						fastgltf::copyFromAccessor<Vector3f>( asset, normalAccessor, normals.data() );
					}
					
					// texture coordinate 0
					if ( uv0Attrib != primitive.attributes.end() )
					{
						fastgltf::Accessor& texCoord0Accessor = asset.accessors[ uv0Attrib->accessorIndex ];
						uv0s.resize( texCoord0Accessor.count );
						fastgltf::copyFromAccessor<Vector2f>( asset, texCoord0Accessor, uv0s.data() );
					}
					
					// vertex colour 0
					if ( colAttrib != primitive.attributes.end() )
					{
						fastgltf::Accessor& colourAccessor = asset.accessors[ colAttrib->accessorIndex ];
						colours.resize( colourAccessor.count );
						fastgltf::copyFromAccessor<Vector3f>( asset, colourAccessor, colours.data() );
					}
				}

				meshAsset.indices        .insert( meshAsset.indices.end(),         indices.begin(),   indices.end() );
				meshAsset.vertexPositions.insert( meshAsset.vertexPositions.end(), positions.begin(), positions.end() );
				meshAsset.vertexNormals  .insert( meshAsset.vertexNormals.end(),   normals.begin(),   normals.end() );
				meshAsset.vertexUVs      .insert( meshAsset.vertexUVs.end(),       uv0s.begin(),      uv0s.end() );
				meshAsset.vertexColours  .insert( meshAsset.vertexColours.end(),   colours.begin(),   colours.end() );

				meshAsset.surfaces.push_back( surface );
			}
		}

		res = m_meshAssets.emplace( meshAsset );
	}

	if( res.isValid() )
		m_assetManager->notifyAssetLoad( res, _path );

	return res;
}

void wv::MeshAssetLoader::unload( ResourceID _resource )
{
	if ( !m_meshAssets.contains( _resource ) )
	{
		return;
	}

	std::filesystem::path path = m_meshAssets.at( _resource ).path;
	int refcount = m_assetManager->notifyAssetUnload( path );

	if ( refcount <= 0 )
	{
		MeshAsset& asset = m_meshAssets.at( _resource );
		asset = {};
		m_meshAssets.erase( _resource );
	}
}
