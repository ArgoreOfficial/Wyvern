#pragma once

#include <cm/Framework/Shader.h>
#include <cm/Framework/Texture.h>
#include <wv/Core/iSingleton.h>

#include <string>
#include <vector>

namespace wv
{
	class cMaterial;
	class cShader;

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
	};
}