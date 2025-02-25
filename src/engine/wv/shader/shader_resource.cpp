#include "shader_resource.h"

#include <wv/filesystem/file_system.h>
#include <wv/memory/memory.h>

#include <auxiliary/json/json11.hpp>
#include <wv/engine.h>
#include <wv/graphics/graphics_device.h>

#include <wv/job/job_system.h>
#include <wv/resource/resource_registry.h>

void wv::ShaderResource::load( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice )
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

	mVertSource.data = _pFileSystem->loadMemory( vsPath );
	mFragSource.data = _pFileSystem->loadMemory( fsPath );

	Job::JobFunction_t fptr = [&]( void* _pUserData )
		{
			Engine* app = Engine::get();
			IGraphicsDevice* pGraphicsDevice = app->graphics;

			ShaderModuleDesc vsDesc;
			vsDesc.source = mVertSource;
			vsDesc.type = WV_SHADER_TYPE_VERTEX;

			ShaderModuleDesc fsDesc;
			fsDesc.source = mFragSource;
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
			mPipelineID           = pGraphicsDevice->createPipeline( desc );

			setComplete( true );
		};

	Job* job = pJobSystem->createJob( JobThreadType::kRENDER, fptr );
	pJobSystem->submit( { job } );
}

void wv::ShaderResource::unload( IFileSystem* _pFileSystem, IGraphicsDevice* _pGraphicsDevice )
{
	setComplete( false );

	_pFileSystem->unloadMemory( mFragSource.data );
	_pFileSystem->unloadMemory( mVertSource.data );

	if( !mPipelineID.is_valid() )
		return;

	JobSystem* pJobSystem = Engine::get()->m_pJobSystem;
	Fence* pFence = Engine::get()->m_pResourceRegistry->getResourceFence();

	PipelineID id = mPipelineID;
	Job::JobFunction_t fptr = [=]( void* )
		{
			_pGraphicsDevice->destroyPipeline( id );
		};
		
	Job* job = pJobSystem->createJob( JobThreadType::kRENDER, fptr, pFence );
	pJobSystem->submit( { job } );
	
}

void wv::ShaderResource::bind( IGraphicsDevice* _pGraphicsDevice )
{
	if ( !mPipelineID.is_valid() )
		return;

	_pGraphicsDevice->cmdBindPipeline( {}, mPipelineID );
}

wv::GPUBufferID wv::ShaderResource::getShaderBuffer( const std::string& _name )
{
	if ( !m_complete )
		return GPUBufferID{ GPUBufferID::InvalidID };

	IGraphicsDevice* pGraphics = Engine::get()->graphics;

	/// this needs to be reworked

	Pipeline& pipeline = pGraphics->m_pipelines.at( mPipelineID );

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