#include "RenderObject.h"

RenderObject::RenderObject( VertexArray* vertexArray, IndexBuffer* indexBuffer, ShaderProgram* shaderProgram, Texture* texture ):
	_vertexArray(vertexArray), _indexBuffer(indexBuffer), _shaderProgram(shaderProgram), _texture(texture)
{ 
}

RenderObject::~RenderObject()
{ 
	delete _vertexArray;
	delete _indexBuffer;
	delete _shaderProgram;
	delete _texture;
}

RenderObject* RenderObject::CreateFromFiles( std::string meshPath, std::string shaderPath, std::string texturePath )
{
	// load mesh file
	std::vector<std::vector<float>> vertexPositions;
	std::vector<glm::vec2> vertexUVs;
	std::vector<unsigned int> indices;

	std::ifstream stream( meshPath );
	std::string line;

	while ( std::getline( stream, line ) )
	{
		// split by spaces
		std::vector<std::string> keys;
		std::istringstream f( line );
		std::string s;
		while ( getline( f, s, ' ' ) )
		{
			keys.push_back( s );
		}

		if ( keys[ 0 ] == "v" ) // vertex positions
		{
			vertexPositions.push_back( { stof( keys[ 1 ] ), stof( keys[ 2 ] ), stof( keys[ 3 ] ), 0.0f, 0.0f } );
		}
		else if ( keys[ 0 ] == "vt" ) // vertex uvs
		{
			vertexUVs.push_back( glm::vec2( stof( keys[ 1 ] ), stof( keys[ 2 ] ) ) );
		}
		else if ( keys[ 0 ] == "f" ) // face
		{
			// split by slashes
			std::vector<std::string> facekeys;
			std::istringstream facekey( keys[1] );
			std::string fs;
			while ( getline( facekey, fs, '/' ) )
			{
				facekeys.push_back( fs );
			}

			indices.push_back( stof( facekeys[ 1 ] ) - 1 );
			// vertexPositions[ stof( keys[ 1 ] ) - 1 ][3] = vertexUVs[ stof( keys[ 2 ] ) - 1 ].x; // add uvs 
			// vertexPositions[ stof( keys[ 1 ] ) - 1 ][4] = vertexUVs[ stof( keys[ 2 ] ) - 1 ].y;
		}
	}

	std::vector<float> vertices;
	for ( int i = 0; i < vertexPositions.size(); i++ )
	{
		vertices.push_back( vertexPositions[ i ][ 0 ] );
		vertices.push_back( vertexPositions[ i ][ 1 ] );
		vertices.push_back( vertexPositions[ i ][ 2 ] );
		// vertices.push_back( vertexPositions[ i ][ 3 ] );
		// vertices.push_back( vertexPositions[ i ][ 4 ] );
	}

	/*
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	unsigned int indices[ 6 ]{
		0, 1, 2,
		2, 3, 0
	};
	*/


	ShaderSource shaderSource( shaderPath );
	ShaderProgram* shaderProgram = new ShaderProgram( shaderSource );

	VertexArray* vertexArray = new VertexArray();
	VertexBuffer* vertexBuffer = new VertexBuffer( &vertices[0], vertices.size() * sizeof(float)); // 5 values(xyz uv)

	VertexBufferLayout layout;
	layout.Push<float>( 3 ); // pos
	// layout.Push<float>( 2 ); // uv
	vertexArray->AddBuffer( *vertexBuffer, layout );

	IndexBuffer* indexBuffer = new IndexBuffer( &indices[0], indices.size() );

	Texture* texture = new Texture( texturePath );
	texture->Bind();
	shaderProgram->SetUniform1i( "u_Texture", 0 );

	RenderObject* renderObject = new RenderObject( vertexArray, indexBuffer, shaderProgram, texture );
	return renderObject;
}
