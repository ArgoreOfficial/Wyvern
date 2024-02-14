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
		
		cm::Shader::hShaderProgram loadShaderProgram( const std::string& _path );
		cm::sTexture2D             loadTexture      ( const std::string& _path );

		cMaterial* loadMaterial( const std::string& _path );
		cShader*   loadShader  ( const std::string& _path );
		cModel*    loadModel   ( const std::string& _path );

	private:

		void processAssimpNode( aiNode* _node, const aiScene* _scene, cModel* _model );
		cMesh* processAssimpMesh( aiMesh* _assimp_mesh, const aiScene* _scene, const std::string& _directory );

	};
}