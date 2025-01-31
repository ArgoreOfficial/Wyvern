#include "ShaderResource.h"

#include <wv/Memory/FileSystem.h>

#include <wv/Auxiliary/json/json11.hpp>
#include <wv/Engine/Engine.h>
#include <wv/Graphics/Graphics.h>

void wv::cShaderResource::load( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics )
{
	Debug::Print( Debug::WV_PRINT_DEBUG, "Loading Shader '%s'\n", m_name.c_str() );

	std::string ext;

#ifdef WV_PLATFORM_WINDOWS
	ext = ".glsl";
#elif defined( WV_PLATFORM_PSVITA )
	ext = "_cg.gxp";
	basepath += "psvita/";
#endif

	std::string vsPath = _pFileSystem->getFullPath( m_name + "_vs" + ext );
	std::string fsPath = _pFileSystem->getFullPath( m_name + "_fs" + ext );

	m_vsSource.data = _pFileSystem->loadMemory( vsPath );
	m_fsSource.data = _pFileSystem->loadMemory( fsPath );

	sProgramDesc vsDesc;
	vsDesc.source = m_vsSource;
	vsDesc.type = WV_SHADER_TYPE_VERTEX;

	sProgramDesc fsDesc;
	fsDesc.source = m_fsSource;
	fsDesc.type = WV_SHADER_TYPE_FRAGMENT;

	sPipelineDesc desc;
	desc.name = m_name;

	/// TODO: generalize 
	wv::sVertexAttribute attributes[] = {
			{ "aPosition",  3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
			{ "aNormal",    3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
			{ "aTangent",   3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
			{ "aColor",     4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
			{ "aTexCoord0", 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
	};
	wv::sVertexLayout layout;
	layout.elements = attributes;
	layout.numElements = 5;

	desc.pVertexLayout = &layout;

	CmdBufferID cmdBuffer = _pLowLevelGraphics->getCommandBuffer();
	desc.vertexProgramID   = _pLowLevelGraphics->cmdCreateProgram( cmdBuffer, vsDesc );
	desc.fragmentProgramID = _pLowLevelGraphics->cmdCreateProgram( cmdBuffer, fsDesc );

	m_pipelineID = _pLowLevelGraphics->cmdCreatePipeline( cmdBuffer, desc );

	auto cb = []( void* _c ) 
		{ 
			iResource* res = (iResource*)_c;
			res->setComplete( true ); 
		};
	_pLowLevelGraphics->setCommandBufferCallback( cmdBuffer, cb, (void*)this );

	_pLowLevelGraphics->submitCommandBuffer( cmdBuffer );

	if( _pLowLevelGraphics->getThreadID() == std::this_thread::get_id() )
		_pLowLevelGraphics->executeCommandBuffer( cmdBuffer );
}

void wv::cShaderResource::unload( cFileSystem* _pFileSystem, iLowLevelGraphics* _pLowLevelGraphics )
{
	setComplete( false );

	_pFileSystem->unloadMemory( m_fsSource.data );
	_pFileSystem->unloadMemory( m_vsSource.data );

	_pLowLevelGraphics->destroyPipeline( m_pipelineID );
}

void wv::cShaderResource::bind( iLowLevelGraphics* _pLowLevelGraphics )
{
	if ( !m_pipelineID.is_valid() )
		return;

	_pLowLevelGraphics->bindPipeline( m_pipelineID );
}

wv::GPUBufferID wv::cShaderResource::getShaderBuffer( const std::string& _name )
{
	if ( !m_complete )
		return GPUBufferID{ GPUBufferID::InvalidID };

	iLowLevelGraphics* pGraphics = cEngine::get()->graphics;

	/// this needs to be reworked

	sPipeline& pipeline = pGraphics->m_pipelines.at( m_pipelineID );

	sProgram& vs = pGraphics->m_programs.at( pipeline.vertexProgramID );
	sProgram& fs = pGraphics->m_programs.at( pipeline.fragmentProgramID );

	for( GPUBufferID bufID : vs.shaderBuffers )
	{
		sGPUBuffer& buf = pGraphics->m_gpuBuffers.at( bufID );
		if( buf.name == _name )
			return bufID;
	}
	
	for( GPUBufferID bufID : fs.shaderBuffers )
	{
		sGPUBuffer& buf = pGraphics->m_gpuBuffers.at( bufID );
		if ( buf.name == _name )
			return bufID;
	}

	return GPUBufferID::InvalidID;
}