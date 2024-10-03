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

		[[nodiscard]] uint32_t getCommandBuffer();
		
		void submitCommandBuffer( uint32_t& _buffer );
		void executeCommandBuffer( uint32_t _index );

		template<typename R, typename T>
		void bufferCommand( uint32_t& _rBuffer, const eGPUTaskType& _type, R** _ppReturn, T* _pInfo );
		template<typename T>
		void bufferCommand( uint32_t& _rBuffer, const eGPUTaskType& _type, T* _pInfo ) { bufferCommand<T, T>( _rBuffer, _type, nullptr, _pInfo ); }
		void bufferCommand( uint32_t& _rBuffer, const eGPUTaskType& _type ) { bufferCommand<char, char>( _rBuffer, _type, nullptr, nullptr ); }

		void setCommandBufferCallback( uint32_t& _buffer, wv::Function<void, void*>::fptr_t _func, void* _caller );
		cMaterial* getEmptyMaterial() { return m_pEmptyMaterial; }

		virtual void terminate() = 0;

		virtual void onResize( int _width, int _height ) = 0;
		virtual void setViewport( int _width, int _height ) = 0;

		virtual void beginRender();
		virtual void endRender();

		virtual RenderTargetID createRenderTarget( sRenderTargetDesc* _desc ) = 0;
		virtual void destroyRenderTarget( RenderTargetID _renderTargetID ) = 0;
		virtual void setRenderTarget( RenderTargetID _renderTargetID ) = 0;

		virtual void setClearColor( const wv::cColor& _color ) = 0;
		virtual void clearRenderTarget( bool _color, bool _depth ) = 0;

		virtual ShaderProgramID createProgram( sShaderProgramDesc* _desc ) = 0;
		virtual void destroyProgram( ShaderProgramID _programID ) = 0;

		virtual PipelineID createPipeline ( sPipelineDesc* _desc ) = 0;
		virtual void       destroyPipeline( PipelineID _pipelineID ) = 0;
		virtual void       bindPipeline   ( PipelineID _pipelineID ) = 0;

		virtual GPUBufferID createGPUBuffer ( sGPUBufferDesc* _desc ) = 0;
		virtual void        allocateBuffer  ( GPUBufferID _buffer, size_t _size ) = 0;
		virtual void        bufferData      ( GPUBufferID _buffer ) = 0;
		virtual void        destroyGPUBuffer( GPUBufferID _buffer ) = 0;
		
		virtual sMesh* createMesh( sMeshDesc* _desc ) = 0;
		virtual void   destroyMesh( sMesh* _pMesh ) = 0;

		virtual TextureID createTexture    ( sTextureDesc* _pDesc )                    = 0;
		virtual void      bufferTextureData( TextureID _pTexture, void* _pData, bool _generateMipMaps ) = 0;
		virtual void      destroyTexture   ( TextureID _pTexture )                     = 0;
		virtual void      bindTextureToSlot( TextureID _pTexture, unsigned int _slot ) = 0;

		virtual void bindVertexBuffer( sMesh* _pMesh, cPipelineResource* _pPipeline ) = 0;

		virtual void setFillMode( eFillMode _mode ) = 0;

		virtual void draw( sMesh* _pMesh ) = 0;
		virtual void drawIndexed( uint32_t _numIndices ) = 0;
		virtual void drawIndexedInstances( uint32_t _numIndices, uint32_t _numInstances ) = 0;

///////////////////////////////////////////////////////////////////////////////////////
		
		cObjectContainer<sShaderProgram, ShaderProgramID> m_shaderPrograms;
		cObjectContainer<sPipeline,      PipelineID>      m_pipelines;
		cObjectContainer<sRenderTarget,  RenderTargetID>  m_renderTargets;
		cObjectContainer<cGPUBuffer,     GPUBufferID>     m_gpuBuffers;
		cObjectContainer<sTexture,       TextureID>       m_textures;

	protected:

		iGraphicsDevice();

		std::thread::id m_threadID;

		virtual bool initialize( GraphicsDeviceDesc* _desc ) = 0;

		GraphicsAPI    m_graphicsApi = WV_GRAPHICS_API_NONE;
		GenericVersion m_graphicsApiVersion{};

		std::mutex m_mutex;
		bool m_reallocatingCommandBuffers = false;

		std::vector<cCommandBuffer> m_commandBuffers;
		std::queue <uint32_t>       m_availableCommandBuffers;
		std::vector<uint32_t>       m_recordingCommandBuffers;
		std::vector<uint32_t>       m_submittedCommandBuffers;

		cMaterial* m_pEmptyMaterial = nullptr;
	};
	template<typename R, typename T>
	inline void iGraphicsDevice::bufferCommand( uint32_t& _rBuffer, const eGPUTaskType& _type, R** _ppReturn, T* _pInfo )
	{
		m_mutex.lock();
		m_commandBuffers[ _rBuffer ].push<R, T>( _type, _ppReturn, _pInfo );
		m_mutex.unlock();
	}
}