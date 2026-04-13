#include "mesh.h"

#include <wv/application.h>
#include <wv/debug/log.h>
#include <wv/filesystem/file_system.h>
#include <wv/rendering/renderer.h>
#include <string>

#include <fstream>

wv::MeshAsset::MeshAsset( const std::filesystem::path& _path )
{
	GeometrySurface surface = deserialize( _path );
	initialize( surface );

	m_path = _path;
}

wv::MeshAsset::~MeshAsset()
{
	Renderer* renderer = Application::getSingleton()->getRenderer();
	renderer->deallocateMesh( m_gpuAllocation );
}

void wv::MeshAsset::serialize( const std::filesystem::path& _path )
{
	std::filesystem::path bindataPath = _path;
	bindataPath.replace_extension( ".wvb" );

	std::ofstream stream{ bindataPath, std::ios::binary };
	// std::ios::app | 
	size_t primCount   = m_surface.primitives.size();
	size_t vertexCount = m_surface.vertexCount();
	size_t indexCount  = m_surface.indexCount();

#define STREAM_WRITE(_v) stream.write( (char*)&_v, sizeof( _v ) )

	STREAM_WRITE( primCount );
	STREAM_WRITE( indexCount );
	STREAM_WRITE( vertexCount );

	for ( size_t i = 0; i < primCount; i++ )
	{
		STREAM_WRITE( m_surface.primitives[ i ].firstIndex );
		STREAM_WRITE( m_surface.primitives[ i ].vertexOffset );
		STREAM_WRITE( m_surface.primitives[ i ].indexCount );
		STREAM_WRITE( m_surface.primitives[ i ].vertexCount );
		STREAM_WRITE( m_surface.primitives[ i ].material );
	}

	for ( size_t i = 0; i < indexCount; i++ ) 
		STREAM_WRITE( m_surface.indices[ i ] );
		
	for ( size_t i = 0; i < vertexCount; i++ )
	{
		STREAM_WRITE( m_surface.vertexPositions[ i ].x );
		STREAM_WRITE( m_surface.vertexPositions[ i ].y );
		STREAM_WRITE( m_surface.vertexPositions[ i ].z );
	}

	for ( size_t i = 0; i < vertexCount; i++ )
	{
		STREAM_WRITE( m_surface.vertexNormals[ i ].x );
		STREAM_WRITE( m_surface.vertexNormals[ i ].y );
		STREAM_WRITE( m_surface.vertexNormals[ i ].z );
	}
		
	for ( size_t i = 0; i < vertexCount; i++ )
	{
		STREAM_WRITE( m_surface.vertexUVs[ i ].x );
		STREAM_WRITE( m_surface.vertexUVs[ i ].y );
	}

	for ( size_t i = 0; i < vertexCount; i++ )
	{
		STREAM_WRITE( m_surface.vertexColours[ i ].x );
		STREAM_WRITE( m_surface.vertexColours[ i ].y );
		STREAM_WRITE( m_surface.vertexColours[ i ].z );
	}
}

wv::GeometrySurface wv::MeshAsset::deserialize( const std::filesystem::path& _path )
{
	std::filesystem::path extension = _path.extension();

	if ( extension == ".gltf" || extension == ".glb" )
	{
		WV_LOG_ERROR( "Use MeshImporterGLTF to deserialize GLTF files\n" );
		return {};
	}
	else if ( extension == ".wvb" )
	{
		std::filesystem::path fullpath = getApp()->getFileSystem()->getFullPath( _path );
		
		std::ifstream stream{ fullpath, std::ios::binary };
		
		GeometrySurface surface;

		size_t primCount = 0;
		size_t indexCount = 0;
		size_t vertexCount = 0;

		if ( !stream )
		{
			WV_LOG_ERROR( "Failed to load mesh %s\n", _path.string().c_str() );
			return {};
		}

	#define STREAM_READ(_v) stream.read( (char*)&_v, sizeof( _v ) );

		STREAM_READ( primCount );
		STREAM_READ( indexCount );
		STREAM_READ( vertexCount );

		surface.primitives.resize( primCount );
		surface.indices.resize( indexCount );
		surface.vertexPositions.resize( vertexCount );
		surface.vertexNormals.resize( vertexCount );
		surface.vertexUVs.resize( vertexCount );
		surface.vertexColours.resize( vertexCount );

		for ( size_t i = 0; i < primCount; i++ )
		{
			STREAM_READ( surface.primitives[ i ].firstIndex );
			STREAM_READ( surface.primitives[ i ].vertexOffset );
			STREAM_READ( surface.primitives[ i ].indexCount );
			STREAM_READ( surface.primitives[ i ].vertexCount );
			STREAM_READ( surface.primitives[ i ].material );
		}

		for ( size_t i = 0; i < indexCount; i++ )
			STREAM_READ( surface.indices[ i ] );

		for ( size_t i = 0; i < vertexCount; i++ )
		{
			STREAM_READ( surface.vertexPositions[ i ].x );
			STREAM_READ( surface.vertexPositions[ i ].y );
			STREAM_READ( surface.vertexPositions[ i ].z );
		}

		for ( size_t i = 0; i < vertexCount; i++ )
		{
			STREAM_READ( surface.vertexNormals[ i ].x );
			STREAM_READ( surface.vertexNormals[ i ].y );
			STREAM_READ( surface.vertexNormals[ i ].z );
		}

		for ( size_t i = 0; i < vertexCount; i++ )
		{
			STREAM_READ( surface.vertexUVs[ i ].x );
			STREAM_READ( surface.vertexUVs[ i ].y );
		}

		for ( size_t i = 0; i < vertexCount; i++ )
		{
			STREAM_READ( surface.vertexColours[ i ].x );
			STREAM_READ( surface.vertexColours[ i ].y );
			STREAM_READ( surface.vertexColours[ i ].z );
		}
		
		return surface;
	}
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
		else data.color = wv::Vector3f( 1.0f, 1.0f, 1.0f ); // default white

		if ( _geometry.vertexUVs.size() > 0 )     data.texCoord = _geometry.vertexUVs[ i ];

		datas.push_back( data );
	}

	m_gpuAllocation = renderer->allocateMesh( _geometry.indices.size(), _geometry.vertexPositions.size(), sizeof( VertexData ) * datas.size() );
	renderer->uploadMesh( m_gpuAllocation, _geometry.indices.data(), _geometry.vertexPositions.data(), datas.data() );
	
	m_primitives = _geometry.primitives;

	for ( auto& prim : m_primitives )
		if ( prim.material >= 0 )
			m_numMaterials = wv::Math::max( m_numMaterials, prim.material );
	
#ifdef WV_DEBUG
	m_surface = _geometry;
#endif
}
