#pragma once

#include <cm/Framework/Shader.h>
#include <cm/Framework/Texture.h>
#include <wv/Graphics/cMesh.h>
#include <wv/Core/iSingleton.h>

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace wv
{
	class cMaterial;
	class cShader;
	class cModel;

	class cContentManager : public iSingleton<cContentManager>
	{
	public:

		void create() override;

		std::string              loadFileToString( const std::string& _path );
		std::vector<std::string> loadFileToVector( const std::string& _path );
		
		// TODO: change to wv::cTexture
		cm::sTexture2D* getTexture( const std::string& _path, bool _ignore_existing = false );

		cMaterial* getMaterial( const std::string& _path, bool _ignore_existing = false );
		cShader*   getShader  ( const std::string& _path, bool _ignore_existing = false );
		cModel*    getModel   ( const std::string& _path, bool _ignore_existing = false );

		void destroyShader( cShader** _shader ) { }

		int getUniformBlockLocation() { return m_uniform_blocks++; }

		std::string getFilenameFromPath( const std::string& _path );

		void reloadAllShaders();

	private:

		void processAssimpNode( aiNode* _node, const aiScene* _scene, cModel* _model );
		cMesh* processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, const std::string& _directory );

		int m_uniform_blocks = 0;

		std::map<std::string, cShader*> m_shaders;
		std::map<std::string, cm::sTexture2D*> m_textures;
	};
}