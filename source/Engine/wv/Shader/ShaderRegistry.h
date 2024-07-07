#pragma once

#include <wv/Resource/ResourceRegistry.h>

#include <wv/Shader/Shader.h>
#include <vector>

namespace wv
{
	class iGraphicsDevice;
	class cShaderProgram;

	class cShaderRegistry : public iResourceRegistry
	{
	public:
		cShaderRegistry( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) :
			iResourceRegistry{ "ShaderRegistry", _pFileSystem, _pGraphicsDevice },
			m_pGraphicsDevice{ _pGraphicsDevice }
		{ }

		cShader* loadShader( eShaderType _type, const std::string& _name );
		
		cShaderProgram* loadProgramFromWShader( const std::string& _name );
		void batchLoadPrograms( const std::vector<std::string>& _names );
		void unloadShaderProgram( cShaderProgram* _program );

	private:

		iGraphicsDevice* m_pGraphicsDevice;

		std::unordered_map<std::string, cShaderProgram*> m_programs;

	};
}