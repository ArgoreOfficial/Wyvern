#include "ShaderProgram.h"

#include <wv/Memory/FileSystem.h>

#include <wv/Auxiliary/json/json11.hpp>
#include <wv/Engine/Engine.h>
#include <wv/Device/GraphicsDevice.h>

void wv::cPipelineResource::load( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
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

	sShaderProgramDesc vsDesc;
	vsDesc.source = m_vsSource;
	vsDesc.type = WV_SHADER_TYPE_VERTEX;

	sShaderProgramDesc fsDesc;
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

	desc.pVertexLayout    = &layout;
	desc.pVertexProgram   = &m_vs;
	desc.pFragmentProgram = &m_fs;

	uint32_t cmdBuffer = _pGraphicsDevice->getCommandBuffer();
	_pGraphicsDevice->bufferCommand( cmdBuffer, WV_GPUTASK_CREATE_PROGRAM, (void**)&m_vs, &vsDesc );
	_pGraphicsDevice->bufferCommand( cmdBuffer, WV_GPUTASK_CREATE_PROGRAM, (void**)&m_fs, &fsDesc );
	_pGraphicsDevice->bufferCommand( cmdBuffer, WV_GPUTASK_CREATE_PIPELINE, (void**)&m_pipelineID, &desc);

	/// this is disgusting
	auto cb =
		[]( void* _c ) 
		{ 
			iResource* res = (iResource*)_c;
			res->setComplete( true ); 
		};

	_pGraphicsDevice->setCommandBufferCallback( cmdBuffer, cb, (void*)this );

	_pGraphicsDevice->submitCommandBuffer( cmdBuffer );

	if( _pGraphicsDevice->getThreadID() == std::this_thread::get_id() )
		_pGraphicsDevice->executeCommandBuffer( cmdBuffer );
}

void wv::cPipelineResource::unload( cFileSystem* _pFileSystem, iGraphicsDevice* _pGraphicsDevice )
{
	setComplete( false );

	_pFileSystem->unloadMemory( m_fsSource.data );
	_pFileSystem->unloadMemory( m_vsSource.data );

	_pGraphicsDevice->destroyPipeline( m_pipelineID );

	m_vs.invalidate(); // destroyed by destroyPipeline
	m_fs.invalidate();
}

void wv::cPipelineResource::use( iGraphicsDevice* _pGraphicsDevice )
{
	if ( !m_pipelineID.isValid() )
		return;

	_pGraphicsDevice->bindPipeline( m_pipelineID );
}

wv::cGPUBuffer* wv::cPipelineResource::getShaderBuffer( const std::string& _name )
{
	if ( !m_pipelineID.isValid() )
		return nullptr;

	iGraphicsDevice* pGraphics = cEngine::get()->graphics;

	/// this needs to be reworked

	sPipeline& pipeline = pGraphics->m_pipelines.get( m_pipelineID );

	sShaderProgram& vs = pGraphics->m_shaderPrograms.get( pipeline.pVertexProgram );
	sShaderProgram& fs = pGraphics->m_shaderPrograms.get( pipeline.pFragmentProgram );

	for( auto& buf : vs.shaderBuffers )
		if( buf->name == _name )
			return buf;
	
	for ( auto& buf : fs.shaderBuffers )
		if ( buf->name == _name )
			return buf;

	return nullptr;
}