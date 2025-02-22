#pragma once

#include <wv/graphics/Pipeline.h>
#include <wv/resource/resource.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class IGraphicsDevice;

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderResource : public iResource
	{
	public:
		cShaderResource( const std::string& _name, const std::string& _path = "" ) :
			iResource( _name, _path )
		{ }

		void load  ( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics ) override;
		void unload( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics ) override;
		
		void bind( IGraphicsDevice* _pLowLevelGraphics );

		PipelineID  getPipelineID() { return m_pipelineID; }
		GPUBufferID getShaderBuffer( const std::string& _name );

		sShaderProgramSource m_fsSource{};
		sShaderProgramSource m_vsSource{};

	private:
		PipelineID m_pipelineID{};

	};

}