#include "Mesh.h"
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Filesystem/Filesystem.h>
#include <bx/bx.h>
#include <bx/pixelformat.h>
#include <Wyvern/Managers/AssetManager.h>

using namespace WV;

WV::Mesh::Mesh( std::string _path ) :
	IAsset( _path )
{
	cAssetManager::addAssetToLoadQueue( this );
}

inline uint32_t encodeNormalRgba8( float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f )
{
	const float src[] =
	{
		_x, // * 0.5f + 0.5f,
		_y, // * 0.5f + 0.5f,
		_z, // * 0.5f + 0.5f,
		_w, // * 0.5f + 0.5f,
	};
	uint32_t dst;
	bx::packRgba8( &dst, src );
	return dst;
}

void WV::Mesh::load()
{
	if ( !Filesystem::fileExists( m_path, true ) ) return; // check if mesh file exists
	std::string filename = Filesystem::getFilenameFromPath( m_path );

	WVDEBUG( "Loading mesh %s", filename.c_str() );
	objl::Loader loader;
	loader.LoadFile( m_path );

	if ( loader.LoadedMeshes.size() == 0 || loader.LoadedVertices.size() == 0 || loader.LoadedIndices.size() == 0 )
	{
		// file didn't load properly or is empty
		WVERROR( "Could not load mesh %s", filename.c_str() );
		return;
	}

	for ( int i = 0; i < loader.LoadedVertices.size(); i++ )
	{
		objl::Vertex& current = loader.LoadedVertices[ i ];

		Vertex v = {
			current.Position.X,
			current.Position.Y,
			current.Position.Z,
			encodeNormalRgba8( current.TextureCoordinate.X, current.TextureCoordinate.Y, 0 )
			/*
			current.Normal.X,
			current.Normal.Y,
			current.Normal.Z,
			// tangent
			current.TextureCoordinate.X,
			current.TextureCoordinate.Y,
			*/
		};

		m_vertices.push_back( v );
	}
	WVDEBUG( "Loaded vertices (%i)", m_vertices.size() );

	for ( int i = 0; i < loader.LoadedIndices.size(); i++ )
	{
		m_indices.push_back( loader.LoadedIndices[ i ] );
	}
	WVDEBUG( "Loaded indices (%i)", m_indices.size() );

	WVDEBUG( "Model mesh succesfully" );
	m_ready = true;
}