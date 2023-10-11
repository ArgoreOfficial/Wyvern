#include "MeshAsset.h"
#include <Wyvern/Filesystem/Filesystem.h>
#include <vendor/OBJ_Loader.h>


using namespace WV;

bool MeshAsset::load( std::string _path, std::string _name )
{
	if ( !Filesystem::fileExists( _path, true ) ) return false; // check if mesh file exists
	if ( _name == "" ) _name = Filesystem::getFilenameFromPath( _path );

	WVDEBUG( "Loading mesh %s", _name.c_str() );

	objl::Loader loader;
	loader.LoadFile( _path );
	WVDEBUG( "Vertices: %i, Faces: %i", loader.LoadedVertices.size(), loader.LoadedIndices.size() / 3 );

	if ( loader.LoadedMeshes.size() == 0 || loader.LoadedVertices.size() == 0 || loader.LoadedIndices.size() == 0 )
	{ // file didn't load properly or is empty
		WVERROR( "Could not load mesh %s", _name.c_str() );
		return false;
	}

	for ( int i = 0; i < loader.LoadedVertices.size(); i++ )
	{
		m_vertices.push_back( loader.LoadedVertices[ i ].Position.X );
		m_vertices.push_back( loader.LoadedVertices[ i ].Position.Y );
		m_vertices.push_back( loader.LoadedVertices[ i ].Position.Z );
		m_vertices.push_back( loader.LoadedVertices[ i ].TextureCoordinate.X );
		m_vertices.push_back( loader.LoadedVertices[ i ].TextureCoordinate.Y );
	}
	for ( int i = 0; i < loader.LoadedIndices.size(); i++ )
	{
		m_indices.push_back( loader.LoadedIndices[ i ] );
	}

	/*
	VertexArray* vertexArray = new VertexArray;
	VertexBuffer* vertexBuffer = new VertexBuffer( &vertices[ 0 ], vertices.size() * sizeof( float ) ); // 3 values(xyz) * sizeof(float)

	VertexBufferLayout layout;
	layout.Push<float>( 3 ); // pos
	layout.Push<float>( 2 ); // uv
	vertexArray->AddBuffer( *vertexBuffer, layout );

	IndexBuffer* indexBuffer = new IndexBuffer( &indices[ 0 ], indices.size() );

	*_out = new Mesh{ vertexArray, indexBuffer };
	m_meshes.push_back( *_out );
	*/

	return true;
}
