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

	class iGraphicsDevice;

	enum eFillMode
	{
		WV_FILL_MODE_SOLID,
		WV_FILL_MODE_WIREFRAME,
		WV_FILL_MODE_POINTS
	};

	struct sPipeline
	{
		wv::Handle handle;

		std::string name;
		
		sShaderProgram* pVertexProgram;
		sShaderProgram* pFragmentProgram;

		void* pPlatformData;
	};

	struct sPipelineDesc
	{
		std::string name;

		sVertexLayout* pVertexLayout;

		sShaderProgram** pVertexProgram;
		sShaderProgram** pFragmentProgram;

		bool reflect = true;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cProgramPipeline : public iResource
	{
	public:
		cProgramPipeline( const std::string& _name ) :
			iResource( _name, "" )
		{ }

		void load  ( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;
		void unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;
		
		void use( iGraphicsDevice* _pGraphicsDevice );

		cGPUBuffer* getShaderBuffer( const std::string& _name );

		sShaderProgramSource m_fsSource;
		sShaderProgramSource m_vsSource;

		sShaderProgram* m_vs;
		sShaderProgram* m_fs;

		sPipeline* m_pPipeline = nullptr;
	private:

	};

}