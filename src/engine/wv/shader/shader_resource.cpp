#include "shader_resource.h"

#include <wv/memory/file_system.h>
#include <wv/memory/memory.h>

#include <auxiliary/json/json11.hpp>
#include <wv/engine.h>
#include <wv/graphics/graphics_device.h>

#include <wv/job/job_system.h>
#include <wv/resource/resource_registry.h>

void wv::ShaderResource::load( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	JobSystem* pJobSystem = Engine::get()->m_pJobSystem;
	
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

	Job::JobFunction_t fptr = [&]( void* _pUserData )
		{
			Engine* app = Engine::get();
			IGraphicsDevice* pGraphicsDevice = app->graphics;

			ShaderModuleDesc vsDesc;
			vsDesc.source = m_vsSource;
			vsDesc.type = WV_SHADER_TYPE_VERTEX;

			ShaderModuleDesc fsDesc;
			fsDesc.source = m_fsSource;
			fsDesc.type = WV_SHADER_TYPE_FRAGMENT;

			PipelineDesc desc;
			desc.name = m_name;

			/// TODO: generalize 
			wv::VertexAttribute attributes[] = {
					{ "aPosition",  3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f pos
					{ "aNormal",    3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f normal
					{ "aTangent",   3, wv::WV_FLOAT, false, sizeof( float ) * 3 }, // vec3f tangent
					{ "aColor",     4, wv::WV_FLOAT, false, sizeof( float ) * 4 }, // vec4f col
					{ "aTexCoord0", 2, wv::WV_FLOAT, false, sizeof( float ) * 2 }  // vec2f texcoord0
			};
			wv::VertexLayout layout;
			layout.elements = attributes;
			layout.numElements = 5;

			desc.pVertexLayout = &layout;

			desc.vertexProgramID   = pGraphicsDevice->createShaderModule( vsDesc );
			desc.fragmentProgramID = pGraphicsDevice->createShaderModule( fsDesc );
			m_pipelineID           = pGraphicsDevice->createPipeline( desc );

			setComplete( true );
		};

	Job* job = pJobSystem->createJob( JobThreadType::kRENDER, nullptr, nullptr, fptr, this );
	pJobSystem->submit( { job } );
}

void wv::ShaderResource::unload( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	setComplete( false );

	_pFileSystem->unloadMemory( m_fsSource.data );
	_pFileSystem->unloadMemory( m_vsSource.data );

	if( !m_pipelineID.is_valid() )
		return;

	JobSystem* pJobSystem = Engine::get()->m_pJobSystem;
	PipelineID id = m_pipelineID;
		
	Job::JobFunction_t fptr = [=]( void* )
		{
			_pLowLevelGraphics->destroyPipeline( id );
		};
		
	Job* job = pJobSystem->createJob(
		JobThreadType::kRENDER,
		Engine::get()->m_pResourceRegistry->getResourceFence(), // ew
		nullptr, fptr, nullptr );

	pJobSystem->submit( { job } );
	
}

void wv::ShaderResource::bind( IGraphicsDevice* _pLowLevelGraphics )
{
	if ( !m_pipelineID.is_valid() )
		return;

	_pLowLevelGraphics->cmdBindPipeline( {}, m_pipelineID );
}

wv::GPUBufferID wv::ShaderResource::getShaderBuffer( const std::string& _name )
{
	if ( !m_complete )
		return GPUBufferID{ GPUBufferID::InvalidID };

	IGraphicsDevice* pGraphics = Engine::get()->graphics;

	/// this needs to be reworked

	Pipeline& pipeline = pGraphics->m_pipelines.at( m_pipelineID );

	Program& vs = pGraphics->m_programs.at( pipeline.vertexProgramID );
	Program& fs = pGraphics->m_programs.at( pipeline.fragmentProgramID );

	for( GPUBufferID bufID : vs.shaderBuffers )
	{
		GPUBuffer& buf = pGraphics->m_gpuBuffers.at( bufID );
		if( buf.name == _name )
			return bufID;
	}
	
	for( GPUBufferID bufID : fs.shaderBuffers )
	{
		GPUBuffer& buf = pGraphics->m_gpuBuffers.at( bufID );
		if ( buf.name == _name )
			return bufID;
	}

	return GPUBufferID::InvalidID;
}