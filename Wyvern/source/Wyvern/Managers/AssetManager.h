#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <vendor/OBJ_Loader.h>
#include <Wyvern/Logging/Logging.h>
#include <Wyvern/Filesystem/Filesystem.h>
#include <Wyvern/Rendering/Framework/Model.h>
#include <vector>

namespace WV
{
	class AssetManager : ISingleton<AssetManager>
	{
	public:
		static void unloadAll() { getInstance().internalUnloadAll(); }

		template<class T>
		static T* load( T** _out, const char* _path ) { return getInstance().internalLoad<T>( _out, _path ); }

		// TEMPORARY
		static void loadModel( Model** _out, const char* _meshPath, const char* _texturePath ) { getInstance().internalLoadModel( _out, _meshPath, _texturePath ); }

	private:
		template<class T>
		T* internalLoad( T** _out, const char* _path );

		void internalLoadModel( Model** _out, const char* _meshPath, const char* _texturePath );

		void internalUnloadAll();
		
		std::vector<Model> m_renderObjects;
		std::vector<Mesh*> m_meshes;
		std::vector<Material*> m_materials;
		std::vector<ShaderProgram*> m_shaderPrograms;
		std::vector<Texture*> m_textures;

		int loadingFiles = 0;
	};



	// ------------------- load templates ------------------- //

	template<class T>
	inline T* AssetManager::internalLoad( T** _out, const char* _path )
	{
		WVERROR("Invalid load type ")
		return nullptr;
	}

	template<>
	inline Mesh* AssetManager::internalLoad<Mesh>( Mesh** _out, const char* _path )
	{
		if ( !Filesystem::fileExists( _path, true ) ) return nullptr; // check if mesh file exists

		objl::Loader loader;
		loader.LoadFile( _path );

		if ( loader.LoadedMeshes.size() == 0 || loader.LoadedVertices.size() == 0 || loader.LoadedIndices.size() == 0 )
		{ // file didn't load properly or is empty
			WVERROR( "Could not load mesh %s", _path );
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

		VertexArray* vertexArray = new VertexArray;
		VertexBuffer* vertexBuffer = new VertexBuffer( &vertices[ 0 ], vertices.size() * sizeof( float ) ); // 3 values(xyz) * sizeof(float)

		VertexBufferLayout layout;
		layout.Push<float>( 3 ); // pos
		layout.Push<float>( 2 ); // uv
		vertexArray->AddBuffer( *vertexBuffer, layout );

		IndexBuffer* indexBuffer = new IndexBuffer( &indices[ 0 ], indices.size() );

		*_out = new Mesh{ vertexArray, indexBuffer };
		m_meshes.push_back( *_out );

	 
		return *_out;
	}

	template<>
	inline Material* AssetManager::internalLoad<Material>( Material** _out, const char* _path )
	{

	}

	template<>
	inline ShaderProgram* AssetManager::internalLoad<ShaderProgram>( ShaderProgram** _out, const char* _path )
	{
		if ( !Filesystem::fileExists( _path, true ) ) return nullptr; // check if shader file exists

		ShaderSource shaderSource( _path );
		*_out = new ShaderProgram( shaderSource );

		m_shaderPrograms.push_back( *_out );
		return *_out;
	}

	template<>
	inline Texture* AssetManager::internalLoad<Texture>( Texture** _out, const char* _path )
	{
		if ( !Filesystem::fileExists( _path, true ) ) return nullptr; // check if texture file exists

		*_out = new Texture( _path );
		(*_out)->Bind();
		// shaderProgram->SetUniform1i( "u_Texture", 0 );

		m_textures.push_back( *_out );
		return *_out;
	}
	
}
