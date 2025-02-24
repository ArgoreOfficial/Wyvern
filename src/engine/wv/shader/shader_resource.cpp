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
	
	Job::JobFunction_t fptr = []( void* _pUserData )
		{
			Engine* app = Engine::get();
			FileSystem* pFileSystem = app->m_pFileSystem;
			IGraphicsDevice* pGraphicsDevice = app->graphics;

			wv::ShaderResource* _this = (wv::ShaderResource*)_pUserData;

			Debug::Print( Debug::WV_PRINT_DEBUG, "Loading Shader '%s'\n", _this->m_name.c_str() );

			std::string ext;

		#ifdef WV_PLATFORM_WINDOWS
			ext = ".glsl";
		#elif defined( WV_PLATFORM_PSVITA )
			ext = "_cg.gxp";
			basepath += "psvita/";
		#endif

			std::string vsPath = _this->m_name + "_vs" + ext;
			std::string fsPath = _this->m_name + "_fs" + ext;

			_this->m_vsSource.data = pFileSystem->loadMemory( vsPath );
			_this->m_fsSource.data = pFileSystem->loadMemory( fsPath );

			ShaderModuleDesc vsDesc;
			vsDesc.source = _this->m_vsSource;
			vsDesc.type = WV_SHADER_TYPE_VERTEX;

			ShaderModuleDesc fsDesc;
			fsDesc.source = _this->m_fsSource;
			fsDesc.type = WV_SHADER_TYPE_FRAGMENT;

			PipelineDesc desc;
			desc.name = _this->m_name;

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
			_this->m_pipelineID    = pGraphicsDevice->createPipeline( desc );

			_this->setComplete( true );
		};

	Job* job = pJobSystem->createJob( JobThreadType::kRENDER, nullptr, nullptr, fptr, this );
	pJobSystem->submit( { job } );
}

void wv::ShaderResource::unload( FileSystem* _pFileSystem, IGraphicsDevice* _pLowLevelGraphics )
{
	setComplete( false );

	_pFileSystem->unloadMemory( m_fsSource.data );
	_pFileSystem->unloadMemory( m_vsSource.data );

	if( m_pipelineID.is_valid() )
	{
		JobSystem* pJobSystem = Engine::get()->m_pJobSystem;
		
		struct PipelineIDData { PipelineID id; };
		PipelineIDData* data = WV_NEW( PipelineIDData );
		data->id = m_pipelineID;

		Job::JobFunction_t fptr = []( void* _pUserData )
			{
				Engine* app = Engine::get();
				PipelineIDData* data = (PipelineIDData*)_pUserData;

				app->graphics->destroyPipeline( data->id );
				WV_FREE( data );
			};
		
		Job* job = pJobSystem->createJob(
			JobThreadType::kRENDER,
			Engine::get()->m_pResourceRegistry->getResourceFence(), // ew
			nullptr,
			fptr,
			data );

		pJobSystem->submit( { job } );
	}
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