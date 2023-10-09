#include "RenderObject.h"
#include <Wyvern/Logging/Logging.h>
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Rendering/Camera.h>
#include <Wyvern/Filesystem/Filesystem.h>

using namespace WV;

RenderObject::RenderObject( VertexArray* _vertexArray, IndexBuffer* _indexBuffer, ShaderProgram* _shaderProgram, Texture* _texture ) :
	m_vertexArray( _vertexArray ), m_indexBuffer( _indexBuffer ), m_shaderProgram( _shaderProgram ), m_texture( _texture )
{
}

RenderObject::~RenderObject()
{
	delete m_vertexArray;
	delete m_indexBuffer;
	delete m_shaderProgram;
	delete m_texture;
}

void WV::RenderObject::bind( Camera& _camera )
{
	m_shaderProgram->Bind();

	glm::mat4 model;
	model = glm::rotate( model, glm::radians( m_rotation.x ), glm::vec3( 1.0, 0.0, 0.0 ) );
	model = glm::rotate( model, glm::radians( m_rotation.y ), glm::vec3( 0.0, 1.0, 0.0 ) );
	model = glm::rotate( model, glm::radians( m_rotation.z ), glm::vec3( 0.0, 0.0, 1.0 ) );
	model = glm::translate( model, m_position );

	m_shaderProgram->SetUniform1i( "u_Texture", 0 );
	m_shaderProgram->SetUniformMat4( "u_Projection", _camera.getProjectionMatrix() );
	m_shaderProgram->SetUniformMat4( "u_View", _camera.getViewMatrix() );
	m_shaderProgram->SetUniformMat4( "u_Model", model );

	m_vertexArray->Bind();
	m_indexBuffer->Bind();
	m_texture->Bind();
}

RenderObject* RenderObject::createFromFiles( std::string meshPath, std::string shaderPath, std::string texturePath )
{
	WVDEBUG( "Creating RenderObject...", meshPath.c_str() );
	
	if ( !Filesystem::fileExists( meshPath, true ) ) return nullptr; // check if mesh file exists
	
	objl::Loader loader;
	loader.LoadFile( meshPath );

	if ( loader.LoadedMeshes.size() == 0 || loader.LoadedVertices.size() == 0 || loader.LoadedIndices.size() == 0 )
	{ // file didn't load properly or is empty
		WVERROR( "Could not load mesh %s", meshPath );
		return nullptr;
	}

	WVDEBUG( "Loaded mesh %s", loader.LoadedMeshes[ 0 ].MeshName.c_str() );
	WVDEBUG( "Vertices: %i, Faces: %i", loader.LoadedVertices.size(), loader.LoadedIndices.size() / 3 );

	std::vector<unsigned int> indices;
	std::vector<float> vertices;

	for ( int i = 0; i < loader.LoadedVertices.size(); i++ )
	{
		vertices.push_back( loader.LoadedVertices[ i ].Position.X );
		vertices.push_back( loader.LoadedVertices[ i ].Position.Y );
		vertices.push_back( loader.LoadedVertices[ i ].Position.Z );
		vertices.push_back( loader.LoadedVertices[ i ].TextureCoordinate.X );
		vertices.push_back( loader.LoadedVertices[ i ].TextureCoordinate.Y );
	}
	for ( int i = 0; i < loader.LoadedIndices.size(); i++ )
	{
		indices.push_back( loader.LoadedIndices[ i ] );
	}


	if ( !Filesystem::fileExists( shaderPath, true ) ) return nullptr; // check if shader file exists

	ShaderSource shaderSource( shaderPath );
	ShaderProgram* shaderProgram = new ShaderProgram( shaderSource );

	VertexArray* vertexArray = new VertexArray();
	VertexBuffer* vertexBuffer = new VertexBuffer( &vertices[ 0 ], vertices.size() * sizeof( float ) ); // 3 values(xyz) * sizeof(float)

	VertexBufferLayout layout;
	layout.Push<float>( 3 ); // pos
	layout.Push<float>( 2 ); // uv
	vertexArray->AddBuffer( *vertexBuffer, layout );

	IndexBuffer* indexBuffer = new IndexBuffer( &indices[ 0 ], indices.size() );
	
	if ( !Filesystem::fileExists( texturePath, true ) ) return nullptr; // check if texture file exists

	Texture* texture = new Texture( texturePath );
	texture->Bind();
	shaderProgram->SetUniform1i( "u_Texture", 0 );

	RenderObject* renderObject = new RenderObject( vertexArray, indexBuffer, shaderProgram, texture );
	return renderObject;
}
