#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Types.h>

#include <wv/Misc/Color.h>
#include <wv/Graphics/GPUBuffer.h>

#include <wv/Graphics/CommandBuffer.h>
#include <wv/Device/GraphicsDevice/ObjectContainer.h>

#include <wv/Shader/ShaderProgram.h>

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <shared_mutex>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class cMaterial;

	struct sMeshDesc;
	struct sMeshNode;
	struct sMesh;
	
	struct sTextureDesc;
	struct sTexture;

	struct iDeviceContext;

	struct sPipeline;
	struct sPipelineDesc;

	struct sShaderProgram;
	struct sShaderProgramDesc;

	struct sRenderTarget;
	struct sRenderTargetDesc;

	struct cPipelineResource;

	struct sGPUBufferDesc;

///////////////////////////////////////////////////////////////////////////////////////

	struct GraphicsDeviceDesc
	{
		GraphicsDriverLoadProc loadProc;
		iDeviceContext* pContext;
	};

///////////////////////////////////////////////////////////////////////////////////////

	class iGraphicsDevice
	{
	public:

		virtual ~iGraphicsDevice() { };

		static iGraphicsDevice* createGraphicsDevice( GraphicsDeviceDesc* _desc );

		void initEmbeds();

		void drawNode( sMeshNode* _node );

		std::thread::id getThreadID() { return m_threadID; }

		[[nodiscard]] CmdBufferID getCommandBuffer();
		
		void submitCommandBuffer( CmdBufferID _bufferID );
		void executeCommandBuffer( CmdBufferID _bufferID );

		template<typename R, typename T>
		void bufferCommand( CmdBufferID _bufferID, const eGPUTaskType& _type, R** _ppReturn, T* _pInfo );
		template<typename T>
		void bufferCommand( CmdBufferID _bufferID, const eGPUTaskType& _type, T* _pInfo ) { bufferCommand<T, T>( _bufferID, _type, nullptr, _pInfo ); }
		void bufferCommand( CmdBufferID _bufferID, const eGPUTaskType& _type ) { bufferCommand<char, char>( _bufferID, _type, nullptr, nullptr ); }

		ShaderProgramID cmdCreateProgram( CmdBufferID _bufferID, const sShaderProgramDesc& _desc );
		PipelineID cmdCreatePipeline( CmdBufferID _bufferID, const sPipelineDesc& _desc );
		RenderTargetID cmdCreateRenderTarget( CmdBufferID _bufferID, const sRenderTargetDesc& _desc );
		GPUBufferID cmdCreateGPUBuffer( CmdBufferID _bufferID, const sGPUBufferDesc& _desc );

		void setCommandBufferCallback( CmdBufferID _bufferID, wv::Function<void, void*>::fptr_t _func, void* _caller );
		cMaterial* getEmptyMaterial() { return m_pEmptyMaterial; }

		virtual void terminate() = 0;

		virtual void onResize( int _width, int _height ) = 0;
		virtual void setViewport( int _width, int _height ) = 0;

		virtual void beginRender();
		virtual void endRender();

		virtual RenderTargetID createRenderTarget( RenderTargetID _renderTargetID, sRenderTargetDesc* _desc ) = 0;
		virtual void destroyRenderTarget( RenderTargetID _renderTargetID ) = 0;
		virtual void setRenderTarget( RenderTargetID _renderTargetID ) = 0;

		virtual void setClearColor( const wv::cColor& _color ) = 0;
		virtual void clearRenderTarget( bool _color, bool _depth ) = 0;

		virtual ShaderProgramID createProgram( ShaderProgramID _shaderID, sShaderProgramDesc* _desc ) = 0;
		virtual void destroyProgram( ShaderProgramID _programID ) = 0;

		virtual PipelineID createPipeline ( PipelineID _pipelineID, sPipelineDesc* _desc ) = 0;
		virtual void       destroyPipeline( PipelineID _pipelineID ) = 0;
		virtual void       bindPipeline   ( PipelineID _pipelineID ) = 0;

		virtual GPUBufferID createGPUBuffer ( GPUBufferID _bufferID, sGPUBufferDesc* _desc ) = 0;
		virtual void        allocateBuffer  ( GPUBufferID _buffer, size_t _size ) = 0;
		virtual void        bufferData      ( GPUBufferID _buffer ) = 0;
		virtual void        destroyGPUBuffer( GPUBufferID _buffer ) = 0;
		
		virtual MeshID createMesh( sMeshDesc* _desc ) = 0;
		virtual void   destroyMesh( MeshID _meshID ) = 0;

		virtual TextureID createTexture    ( sTextureDesc* _pDesc )                    = 0;
		virtual void      bufferTextureData( TextureID _pTexture, void* _pData, bool _generateMipMaps ) = 0;
		virtual void      destroyTexture   ( TextureID _pTexture )                     = 0;
		virtual void      bindTextureToSlot( TextureID _pTexture, unsigned int _slot ) = 0;

		virtual void bindVertexBuffer( MeshID _meshID, cPipelineResource* _pPipeline ) = 0;

		virtual void setFillMode( eFillMode _mode ) = 0;

		virtual void draw( MeshID _meshID ) = 0;
		virtual void drawIndexed( uint32_t _numIndices ) = 0;
		virtual void drawIndexedInstances( uint32_t _numIndices, uint32_t _numInstances ) = 0;

///////////////////////////////////////////////////////////////////////////////////////
		
		cObjectHandleContainer<sShaderProgram, ShaderProgramID> m_shaderPrograms;
		cObjectHandleContainer<sPipeline,      PipelineID>      m_pipelines;
		cObjectHandleContainer<sRenderTarget,  RenderTargetID>  m_renderTargets;
		cObjectHandleContainer<cGPUBuffer,     GPUBufferID>     m_gpuBuffers;
		cObjectHandleContainer<sTexture,       TextureID>       m_textures;
		cObjectHandleContainer<sMesh,          MeshID>          m_meshes;
		
///////////////////////////////////////////////////////////////////////////////////////

	protected:

		iGraphicsDevice();

		std::thread::id m_threadID;

		virtual bool initialize( GraphicsDeviceDesc* _desc ) = 0;

		GraphicsAPI    m_graphicsApi = WV_GRAPHICS_API_NONE;
		GenericVersion m_graphicsApiVersion{};

		std::mutex m_mutex;
		bool m_reallocatingCommandBuffers = false;

		std::vector<cCommandBuffer> m_commandBuffers;
		std::queue <CmdBufferID>    m_availableCommandBuffers;
		std::vector<CmdBufferID>    m_recordingCommandBuffers;
		std::vector<CmdBufferID>    m_submittedCommandBuffers;

		cMaterial* m_pEmptyMaterial = nullptr;
	};

	template<typename R, typename T>
	inline void iGraphicsDevice::bufferCommand( CmdBufferID _bufferID, const eGPUTaskType& _type, R** _ppReturn, T* _pInfo )
	{
		m_mutex.lock();
		m_commandBuffers[ _bufferID.value ].push<R, T>( _type, _ppReturn, _pInfo );
		m_mutex.unlock();
	}
}