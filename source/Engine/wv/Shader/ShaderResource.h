#pragma once

#include <wv/Graphics/Pipeline.h>
#include <wv/Resource/Resource.h>

#include <vector>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class iLowLevelGraphics;

///////////////////////////////////////////////////////////////////////////////////////

	class cShaderResource : public iResource
	{
	public:
		cShaderResource( const std::string& _name, const std::string& _path = "" ) :
			iResource( _name, _path )
		{ }

		void load  ( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics ) override;
		void unload( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics ) override;
		
		void bind( iLowLevelGraphics* _pLowLevelGraphics );

		PipelineID  getPipelineID() { return m_pipelineID; }
		GPUBufferID getShaderBuffer( const std::string& _name );

		sShaderProgramSource m_fsSource{};
		sShaderProgramSource m_vsSource{};

	private:
		PipelineID m_pipelineID{};

	};

}