#include "MeshAsset.h"
#include <Wyvern/Filesystem/Filesystem.h>
#include <vendor/OBJ_Loader.h>

using namespace WV;

bool MeshAsset::load( std::string _path )
{
	if ( !Filesystem::fileExists( _path, true ) ) return false; // check if mesh file exists
	std::string filename = Filesystem::getFilenameFromPath( _path );

	WVDEBUG( "Loading mesh %s", filename.c_str() );

	objl::Loader loader;
	loader.LoadFile( _path );
	
	if ( loader.LoadedMeshes.size() == 0 || loader.LoadedVertices.size() == 0 || loader.LoadedIndices.size() == 0 )
	{ // file didn't load properly or is empty
		WVERROR( "Could not load mesh %s", filename.c_str() );
		return false;
	}

	WVDEBUG( "Pushing vertices (%i)", loader.LoadedVertices.size() );
	for ( int i = 0; i < loader.LoadedVertices.size(); i++ )
	{
		m_vertices.push_back( loader.LoadedVertices[ i ].Position.X );
		m_vertices.push_back( loader.LoadedVertices[ i ].Position.Y );
		m_vertices.push_back( loader.LoadedVertices[ i ].Position.Z );
		m_vertices.push_back( loader.LoadedVertices[ i ].TextureCoordinate.X );
		m_vertices.push_back( loader.LoadedVertices[ i ].TextureCoordinate.Y );
	}
	WVDEBUG( "Pushing indices (%i)", loader.LoadedIndices.size() );
	for ( int i = 0; i < loader.LoadedIndices.size(); i++ )
	{
		m_indices.push_back( loader.LoadedIndices[ i ] );
	}

	WVDEBUG( "Model mesh succesfully" );
	return true;
}
