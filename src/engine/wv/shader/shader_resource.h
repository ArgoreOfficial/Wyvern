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

	class ShaderResource : public IResource
	{
	public:
		ShaderResource( const std::string& _name, const std::string& _path = "" ) :
			IResource( _name, _path )
		{ }

		void load  ( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics ) override;
		void unload( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics ) override;
		
		void bind( IGraphicsDevice* _pLowLevelGraphics );

		PipelineID  getPipelineID() { return m_pipelineID; }
		GPUBufferID getShaderBuffer( const std::string& _name );

		ShaderProgramSource m_fsSource{};
		ShaderProgramSource m_vsSource{};

	private:
		PipelineID m_pipelineID{};

	};

}