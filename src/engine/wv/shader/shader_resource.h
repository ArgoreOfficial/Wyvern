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

		void load  ( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) override;
		void unload( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice ) override;
		
		void bind( IGraphicsDevice* _pGraphicsDevice );

		PipelineID  getPipelineID() { return mPipelineID; }
		GPUBufferID getShaderBuffer( const std::string& _name );

		ShaderProgramSource mFragSource{};
		ShaderProgramSource mVertSource{};

	private:
		PipelineID mPipelineID{};

	};

}