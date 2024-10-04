#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Types.h>

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
		ProgramID vertexProgramID{};
		ProgramID fragmentProgramID{};

		void* pPlatformData;
	};

	struct sPipelineDesc
	{
		std::string name;
		sVertexLayout* pVertexLayout = nullptr;
		ProgramID vertexProgramID{};
		ProgramID fragmentProgramID{};

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
		
		void bind( iGraphicsDevice* _pGraphicsDevice );

		PipelineID  getPipelineID() { return m_pipelineID; }
		GPUBufferID getShaderBuffer( const std::string& _name );

		sShaderProgramSource m_fsSource{};
		sShaderProgramSource m_vsSource{};

	private:
		PipelineID m_pipelineID{};

	};

}