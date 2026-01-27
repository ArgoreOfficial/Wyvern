#include "mesh.h"

#include <wv/application.h>
#include <wv/debug/log.h>
#include <wv/filesystem/file_system.h>
#include <wv/rendering/renderer.h>

#include <string>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

template <>
struct fastgltf::ElementTraits<wv::Vector3f> : fastgltf::ElementTraitsBase<wv::Vector3f, AccessorType::Vec3, float> { };

template <>
struct fastgltf::ElementTraits<wv::Vector2f> : fastgltf::ElementTraitsBase<wv::Vector2f, AccessorType::Vec2, float> { };

wv::MeshAsset::MeshAsset( const std::filesystem::path& _path )
{
	GeometrySurface surface = deserialize( _path );
	initialize( surface );

	m_path = _path;
}

wv::GeometrySurface wv::MeshAsset::deserialize( const std::filesystem::path& _path )
{
	auto extension = _path.extension();
	if ( extension == ".gltf" || extension == ".glb" )
	{
		return deserializeGltf( _path );
	}
	else
	{
		WV_LOG_ERROR( "%s is not a mesh format\n", extension.string().c_str() );
		return {};
	}
}

wv::GeometrySurface wv::MeshAsset::deserializeGltf( const std::filesystem::path& _path )
{
	// Load file

	IFileSystem* fs = Application::getSingleton()->getFileSystem();

	fastgltf::Parser parser;
	std::filesystem::path fullpath = fs->getFullPath( _path );

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

	// Parse gltf

	fastgltf::Asset& asset = load.get();
	GeometrySurface surface;

	for ( fastgltf::Mesh& mesh : asset.meshes )
	{
		for ( auto& primitive : mesh.primitives )
		{
			std::vector<uint16_t> indices;
			std::vector<Vector3f> positions;
			std::vector<Vector3f> normals;
			std::vector<Vector2f> uv0s;
			std::vector<Vector3f> colours;

			fastgltf::Attribute* posAttrib = primitive.findAttribute( "POSITION" );
			fastgltf::Attribute* norAttrib = primitive.findAttribute( "NORMAL" );
			fastgltf::Attribute* uv0Attrib = primitive.findAttribute( "TEXCOORD_0" );
			fastgltf::Attribute* colAttrib = primitive.findAttribute( "COLOR_0" );

			// load indices
			{
				fastgltf::Accessor& indexAccessor = asset.accessors[ primitive.indicesAccessor.value() ];
				indices.resize( indexAccessor.count );
				fastgltf::copyFromAccessor<uint16_t>( asset, indexAccessor, indices.data() );
			}

			// load vertex data
			{
				// vertex position
				if ( posAttrib != primitive.attributes.end() )
				{
					fastgltf::Accessor& positionAccessor = asset.accessors[ posAttrib->accessorIndex ];
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

			surface.indices.insert( surface.indices.end(), indices.begin(), indices.end() );
			surface.vertexPositions.insert( surface.vertexPositions.end(), positions.begin(), positions.end() );
			surface.vertexNormals.insert( surface.vertexNormals.end(), normals.begin(), normals.end() );
			surface.vertexUVs.insert( surface.vertexUVs.end(), uv0s.begin(), uv0s.end() );
			surface.vertexColours.insert( surface.vertexColours.end(), colours.begin(), colours.end() );
		}
	}

	return surface;
}

void wv::MeshAsset::initialize( const GeometrySurface& _geometry )
{
	if ( _geometry.vertexCount() == 0 )
		return;

	Renderer* renderer = Application::getSingleton()->getRenderer();
	std::vector<VertexData> datas;

	for ( size_t i = 0; i < _geometry.vertexPositions.size(); i++ )
	{
		VertexData data;

		if ( _geometry.vertexNormals.size() > 0 ) data.normal = _geometry.vertexNormals[ i ];
		if ( _geometry.vertexColours.size() > 0 ) data.color = _geometry.vertexColours[ i ];
		if ( _geometry.vertexUVs.size() > 0 )     data.texCoord = _geometry.vertexUVs[ i ];

		datas.push_back( data );
	}

	m_gpuAllocation = renderer->createMesh( _geometry.indices, _geometry.vertexPositions, datas.data(), sizeof( VertexData ) * datas.size() );

#ifdef WV_DEBUG
	m_surface = _geometry;
#endif
}
