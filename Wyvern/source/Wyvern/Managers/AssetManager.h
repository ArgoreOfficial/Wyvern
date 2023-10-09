#pragma once
#include <Wyvern/Core/ISingleton.h>
#include <vector>

namespace WV
{
	class RenderObject;
	class Mesh;
	class Material;
	class ShaderProgram;
	class Texture;

	class AssetManager : ISingleton<AssetManager>
	{
	public:
		static Mesh* loadMesh( const char* _path ) { return getInstance().internalLoadMesh( _path ); }
		static Material* loadMaterial( const char* _path ) { return getInstance().internalLoadMaterial( _path ); }
		static ShaderProgram* loadShader( const char* _path ) { return getInstance().internalLoadShader( _path ); }
		static Texture* loadTexture( const char* _path ) { return getInstance().internalLoadTexture( _path ); }

		static void loadModel( RenderObject* _out, const char* _meshPath ) { getInstance().internalLoadModel( _out, _meshPath ); }

		static void unloadAll() { getInstance().internalUnloadAll(); }

	private:
		Mesh* internalLoadMesh( const char* _path );
		Material* internalLoadMaterial( const char* _path );
		ShaderProgram* internalLoadShader( const char* _path );
		Texture* internalLoadTexture( const char* _path );

		void internalLoadModel( RenderObject* _out, const char* _meshPath );

		void internalUnloadAll();
		
		std::vector<RenderObject> m_renderObjects;
		std::vector<Mesh*> m_meshes;
		std::vector<Material*> m_materials;
		std::vector<ShaderProgram*> m_shaderPrograms;
		std::vector<Texture*> m_textures;
	};
}
