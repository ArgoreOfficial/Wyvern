#pragma once

#include <wv/Types.h>

#include <wv/Resource/Resource.h>

#include <wv/Shader/UniformBlock.h>
#include <wv/Shader/Shader.h>

#include <wv/Graphics/VertexLayout.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class sShader;
	class iGraphicsDevice;

	struct sShaderProgramDesc
	{
		std::string name;
		
		sVertexLayout* pVertexLayout;

		sShader* pVertexShader;
		sShader* pFragmentShader;

		bool reflect = true;
	};

	struct sShaderProgram
	{
		wv::Handle handle;

		std::string name;
		
		sShader* pVertexShader;
		sShader* pFragmentShader;

		std::vector<cShaderBuffer*> shaderBuffers;
		std::vector<sUniform> textureUniforms;

		PlatformData pPlatformData;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderProgram : public iResource
	{
	public:
		cShaderProgram( const std::string& _name ) :
			iResource( _name, L"" )
		{ }

		void load  ( cFileSystem* _pFileSystem ) override;
		void unload( cFileSystem* _pFileSystem ) override;
		
		void create ( iGraphicsDevice* _pGraphicsDevice ) override;
		void destroy( iGraphicsDevice* _pGraphicsDevice ) override;

		void use( iGraphicsDevice* _pGraphicsDevice );

		cShaderBuffer* getShaderBuffer( const std::string& _name );

		sShaderSource m_fsSource;
		sShaderSource m_vsSource;
		sShaderProgram* m_pProgram = nullptr;
	private:

	};

}