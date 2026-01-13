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
	// TODO: actual mesh loading lmfao
	ResourceID res{};

	if ( m_assetManager->containsAsset( _path ) )
	{
		res = m_assetManager->getAsset( _path );
	}
	else
	{
		if ( _path == "wv://cube" )
		{
			MeshAsset asset{};

			asset.vertexPositions = {
				{ -1, -1, -1 },
				{  1, -1, -1 },
				{  1,  1, -1 },
				{ -1,  1, -1 },
				{ -1, -1,  1 },
				{  1, -1,  1 },
				{  1,  1,  1 },
				{ -1,  1,  1 }
			};

			asset.indices = {
				0, 1, 3, 3, 1, 2,
				1, 5, 2, 2, 5, 6,
				5, 4, 6, 6, 4, 7,
				4, 0, 7, 7, 0, 3,
				3, 2, 7, 7, 2, 6,
				4, 5, 0, 0, 5, 1
			};

			asset.vertexColours = {
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f },
				{ 0.0f, 0.0f, 0.0f },
			};

			res = m_meshAssets.emplace( asset );
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
			MeshAsset meshAsset{};
			meshAsset.path = _path;
			
			for ( fastgltf::Mesh& mesh : asset.meshes )
			{
				for ( auto& primitive : mesh.primitives )
				{
					GeometrySurface surface;
					surface.startIndex = meshAsset.indices.size();
					surface.count = asset.accessors[ primitive.indicesAccessor.value() ].count;

					// load indices
					{
						fastgltf::Accessor& indexAccessor = asset.accessors[ primitive.indicesAccessor.value() ];
						size_t initialVerticesSize = meshAsset.vertexPositions.size();
						
						fastgltf::iterateAccessor<std::uint32_t>( asset, indexAccessor, 
							[ & ]( std::uint32_t index ) {
								meshAsset.indices.push_back( index + initialVerticesSize );
							} );
					}

					// load vertex data
					{
						// vertex position
						fastgltf::Accessor& positionAccessor = asset.accessors[ primitive.findAttribute( "POSITION" )->accessorIndex ];
						fastgltf::iterateAccessorWithIndex<wv::Vector3f>( asset, positionAccessor, 
							[ & ]( wv::Vector3f pos, std::size_t idx ) {
								meshAsset.vertexPositions.push_back( pos );
							} );
						
						// vertex normal
						fastgltf::Accessor& normalAccessor = asset.accessors[ primitive.findAttribute( "NORMAL" )->accessorIndex ];
						fastgltf::iterateAccessorWithIndex<wv::Vector3f>( asset, normalAccessor,
							[ & ]( wv::Vector3f _normal, std::size_t _idx ) {
								meshAsset.vertexNormals.push_back( _normal );
							} );
						
						// texture coordinate 0
						fastgltf::Accessor& texCoord0Accessor = asset.accessors[ primitive.findAttribute( "TEXCOORD_0" )->accessorIndex ];
						fastgltf::iterateAccessorWithIndex<wv::Vector2f>( asset, texCoord0Accessor,
							[ & ]( wv::Vector2f _texCoord, std::size_t _idx ) {
								meshAsset.vertexUVs.push_back( _texCoord );
							} );

						// vertex colour
						fastgltf::Accessor& colourAccessor = asset.accessors[ primitive.findAttribute( "COLOR_0" )->accessorIndex ];
						fastgltf::iterateAccessorWithIndex<wv::Vector3f>( asset, normalAccessor,
							[ & ]( wv::Vector3f _colour, std::size_t _idx ) {
								meshAsset.vertexColours.push_back( _colour );
							} );
					}
				}
			}

			res = m_meshAssets.emplace( meshAsset );
		}
	}

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
		// TODO: actual mesh unloading
		m_meshAssets.erase( _resource );
	}
}
