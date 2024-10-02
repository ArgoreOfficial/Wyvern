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
		
		ShaderProgramID pVertexProgram;
		ShaderProgramID pFragmentProgram;

		void* pPlatformData;
	};

	WV_DEFINE_ID( PipelineID );

	struct sPipelineDesc
	{
		std::string name;

		sVertexLayout* pVertexLayout = nullptr;

		ShaderProgramID* pVertexProgram = nullptr;
		ShaderProgramID* pFragmentProgram = nullptr;

		bool reflect = true;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class cPipelineResource : public iResource
	{
	public:
		cPipelineResource( const std::string& _name, const std::string& _path = "" ) :
			iResource( _name, _path )
		{ }

		void load  ( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;
		void unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice ) override;
		
		void use( iGraphicsDevice* _pGraphicsDevice );

		cGPUBuffer* getShaderBuffer( const std::string& _name );

		sShaderProgramSource m_fsSource{};
		sShaderProgramSource m_vsSource{};

		ShaderProgramID m_vs{};
		ShaderProgramID m_fs{};

		PipelineID m_pipelineID{};
	private:

	};

}