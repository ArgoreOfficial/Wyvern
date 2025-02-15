#include "ShaderResource.h"

#include <wv/Memory/FileSystem.h>

#include <wv/Auxiliary/json/json11.hpp>
#include <wv/Engine/Engine.h>
#include <wv/Graphics/GraphicsDevice.h>

void wv::cShaderResource::load( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	Debug::Print( Debug::WV_PRINT_DEBUG, "Loading Shader '%s'\n", m_name.c_str() );

	std::string ext;

#ifdef WV_PLATFORM_WINDOWS
	ext = ".glsl";
#elif defined( WV_PLATFORM_PSVITA )
	ext = "_cg.gxp";
	basepath += "psvita/";
#endif

	std::string vsPath = m_name + "_vs" + ext;
	std::string fsPath = m_name + "_fs" + ext;

	m_vsSource.data = _pFileSystem->loadMemory( vsPath );
	m_fsSource.data = _pFileSystem->loadMemory( fsPath );

	ShaderModuleDesc vsDesc;
	vsDesc.source = m_vsSource;
	vsDesc.type = WV_SHADER_TYPE_VERTEX;

	ShaderModuleDesc fsDesc;
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

	desc.vertexProgramID   = _pLowLevelGraphics->createShaderModule( vsDesc );
	desc.fragmentProgramID = _pLowLevelGraphics->createShaderModule( fsDesc );
	m_pipelineID = _pLowLevelGraphics->createPipeline( desc );

	auto cb = []( void* _c ) 
		{ 
			iResource* res = (iResource*)_c;
			res->setComplete( true ); 
		};
	_pLowLevelGraphics->queueAddCallback( cb, (void*)this );
}

void wv::cShaderResource::unload( cFileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	setComplete( false );

	_pFileSystem->unloadMemory( m_fsSource.data );
	_pFileSystem->unloadMemory( m_vsSource.data );

	if( m_pipelineID.is_valid() )
		_pLowLevelGraphics->destroyPipeline( m_pipelineID );
}

void wv::cShaderResource::bind( IGraphicsDevice* _pLowLevelGraphics )
{
	if ( !m_pipelineID.is_valid() )
		return;

	_pLowLevelGraphics->cmdBindPipeline( {}, m_pipelineID );
}

wv::GPUBufferID wv::cShaderResource::getShaderBuffer( const std::string& _name )
{
	if ( !m_complete )
		return GPUBufferID{ GPUBufferID::InvalidID };

	IGraphicsDevice* pGraphics = cEngine::get()->graphics;

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