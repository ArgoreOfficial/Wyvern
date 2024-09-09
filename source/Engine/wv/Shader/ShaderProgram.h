#pragma once

#include <wv/Types.h>

#include <wv/Resource/Resource.h>

#include <wv/Shader/Shader.h>

#include <wv/Graphics/VertexLayout.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class sShaderProgram;
	class iGraphicsDevice;

	struct sPipelineDesc
	{
		std::string name;
		
		sVertexLayout* pVertexLayout;

		sShaderProgram* pVertexProgram;
		sShaderProgram* pFragmentProgram;

		bool reflect = true;
	};

	struct sPipeline
	{
		wv::Handle handle;

		std::string name;
		
		sShaderProgram* pVertexProgram;
		sShaderProgram* pFragmentProgram;

		PlatformData pPlatformData;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cProgramPipeline : public iResource
	{
	public:
		cProgramPipeline( const std::string& _name ) :
			iResource( _name, L"" )
		{ }

		void load  ( cFileSystem* _pFileSystem ) override;
		void unload( cFileSystem* _pFileSystem ) override;
		
		void create ( iGraphicsDevice* _pGraphicsDevice ) override;
		void destroy( iGraphicsDevice* _pGraphicsDevice ) override;

		void use( iGraphicsDevice* _pGraphicsDevice );

		cShaderBuffer* getShaderBuffer( const std::string& _name );

		sShaderProgramSource m_fsSource;
		sShaderProgramSource m_vsSource;
		sPipeline* m_pPipeline = nullptr;
	private:

	};

}