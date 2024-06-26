#pragma once

#include <wv/Resource/ResourceRegistry.h>

#include <wv/Shader/Shader.h>

namespace wv
{
	class iGraphicsDevice;
	class cShaderProgram;

	class cShaderRegistry : public iResourceRegistry
	{
	public:
		cShaderRegistry( cFileSystem* _fileSystem, iGraphicsDevice* _graphicsDevice ) :
			iResourceRegistry{"ShaderRegistry", _fileSystem},
			m_pGraphicsDevice{_graphicsDevice}
		{ }

		cShader* loadShader( eShaderType _type, const std::string& _name );
		
		cShaderProgram* loadProgramFromWShader( const std::string& _name );
		void unloadShaderProgram( cShaderProgram* _program );
		
	private:

		iGraphicsDevice* m_pGraphicsDevice;

		std::unordered_map<std::string, cShaderProgram*> m_programs;

	};
}