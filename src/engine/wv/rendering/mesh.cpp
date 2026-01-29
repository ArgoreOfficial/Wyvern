#include "mesh.h"

#include <wv/application.h>
#include <wv/debug/log.h>
#include <wv/filesystem/file_system.h>
#include <wv/rendering/renderer.h>
#include <string>

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

wv::GeometrySurface wv::MeshAsset::deserialize( const std::filesystem::path& _path )
{
	auto extension = _path.extension();
	if ( extension == ".gltf" || extension == ".glb" )
	{
		WV_LOG_ERROR( "Use MeshImporterGLTF to deserialize GLTF files\n" );
		return {};
	}
	else
	{
		WV_LOG_ERROR( "%s is not a mesh format\n", extension.string().c_str() );
		return {};
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
		if ( _geometry.vertexUVs.size() > 0 )     data.texCoord = _geometry.vertexUVs[ i ];

		datas.push_back( data );
	}

	m_gpuAllocation = renderer->allocateMesh( _geometry.indices, _geometry.vertexPositions, datas.data(), sizeof( VertexData ) * datas.size() );
	m_primitives = _geometry.primitives;

	for ( auto& prim : m_primitives )
		if ( prim.material >= 0 )
			m_numMaterials = wv::Math::max( m_numMaterials, prim.material );
	
#ifdef WV_DEBUG
	m_surface = _geometry;
#endif
}
