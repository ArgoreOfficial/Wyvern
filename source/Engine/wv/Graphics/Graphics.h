#pragma once

#include <wv/Types.h>
#include <wv/Graphics/Types.h>

#include <wv/Misc/Color.h>
#include <wv/Graphics/GPUBuffer.h>

#include <wv/Graphics/CommandBuffer.h>
#include <wv/Graphics/ObjectHandleContainer.h>
#include <wv/Graphics/Pipeline.h>


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

	class iDeviceContext;
	class cShaderResource;
	class cMaterial;

///////////////////////////////////////////////////////////////////////////////////////

	struct sLowLevelGraphicsDesc
	{
		GraphicsDriverLoadProc loadProc;
		iDeviceContext* pContext;
	};

	template<typename T, typename D>
	struct sCmdCreateDesc{ T id; D desc; };

///////////////////////////////////////////////////////////////////////////////////////

	class iLowLevelGraphics
	{
	public:

		virtual ~iLowLevelGraphics() { };

		static iLowLevelGraphics* createGraphics( sLowLevelGraphicsDesc* _desc );

		void initEmbeds();

		void drawNode( sMeshNode* _node );

		std::thread::id getThreadID() { return m_threadID; }

		[[nodiscard]] CmdBufferID getCommandBuffer();
		
		void submitCommandBuffer ( CmdBufferID _bufferID );
		void executeCommandBuffer( CmdBufferID _bufferID );

		template<typename T>
		void cmd( CmdBufferID _bufferID, const eGPUTaskType& _type, T* _pInfo );
		void cmd( CmdBufferID _bufferID, const eGPUTaskType& _type ) { cmd<char>( _bufferID, _type, nullptr ); }

		template<typename ID, typename T>
		ID cmdCreateCommand( CmdBufferID _bufferID, eGPUTaskType _task, ID _id, const T& _desc );

		ProgramID       cmdCreateProgram     ( CmdBufferID _bufferID, const sProgramDesc&      _desc );
		PipelineID      cmdCreatePipeline    ( CmdBufferID _bufferID, const sPipelineDesc&     _desc );
		RenderTargetID  cmdCreateRenderTarget( CmdBufferID _bufferID, const sRenderTargetDesc& _desc );
		GPUBufferID     cmdCreateGPUBuffer   ( CmdBufferID _bufferID, const sGPUBufferDesc&    _desc );
		MeshID          cmdCreateMesh        ( CmdBufferID _bufferID, const sMeshDesc&         _desc );
		TextureID       cmdCreateTexture     ( CmdBufferID _bufferID, const sTextureDesc&      _desc );

		void setCommandBufferCallback( CmdBufferID _bufferID, wv::Function<void, void*>::fptr_t _func, void* _caller );
		
		cMaterial* getEmptyMaterial() { return m_pEmptyMaterial; }

///////////////////////////////////////////////////////////////////////////////////////

		virtual void terminate() = 0;

		virtual void onResize   ( int _width, int _height ) = 0;
		virtual void setViewport( int _width, int _height ) = 0;

		virtual void beginRender();
		virtual void endRender  ();

		virtual RenderTargetID createRenderTarget ( RenderTargetID _renderTargetID, sRenderTargetDesc* _desc ) = 0;
		virtual void           destroyRenderTarget( RenderTargetID _renderTargetID )                           = 0;
		virtual void           setRenderTarget    ( RenderTargetID _renderTargetID )                           = 0;

		virtual void setClearColor    ( const wv::cColor& _color ) = 0;
		virtual void clearRenderTarget( bool _color, bool _depth ) = 0;

		virtual ProgramID createProgram ( ProgramID _programID, sProgramDesc* _desc ) = 0;
		virtual void      destroyProgram( ProgramID _programID )                      = 0;

		virtual PipelineID createPipeline ( PipelineID _pipelineID, sPipelineDesc* _desc ) = 0;
		virtual void       destroyPipeline( PipelineID _pipelineID )                       = 0;
		virtual void       bindPipeline   ( PipelineID _pipelineID )                       = 0;


		virtual GPUBufferID createGPUBuffer ( GPUBufferID _bufferID, sGPUBufferDesc* _desc ) = 0;
		virtual void        destroyGPUBuffer( GPUBufferID _bufferID )                        = 0;
		
		virtual void bufferData   ( GPUBufferID _bufferID, void* _pData, size_t _size )               = 0;
		virtual void bufferSubData( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base ) = 0;

		virtual void copyBufferSubData( GPUBufferID _readBufferID, GPUBufferID _writeBufferID, size_t _readOffset, size_t _writeOffset, size_t _size ) = 0;
		
		virtual MeshID createMesh ( MeshID _meshID, sMeshDesc* _desc );
		virtual void   destroyMesh( MeshID _meshID );

		virtual TextureID createTexture    ( TextureID _textureID, sTextureDesc* _pDesc )                = 0;
		virtual void      bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps ) = 0;
		virtual void      destroyTexture   ( TextureID _textureID )                                      = 0;
		virtual void      bindTextureToSlot( TextureID _textureID, unsigned int _slot )                  = 0;

		virtual void bindVertexBuffer( GPUBufferID _indexBufferID, GPUBufferID _vertexPullBufferID ) = 0;

		virtual void setFillMode( eFillMode _mode ) = 0;

		void draw( MeshID _meshID );

		virtual void draw                ( uint32_t _firstVertex, uint32_t _numVertices ) = 0;
		virtual void drawIndexed         ( uint32_t _numIndices )                         = 0;
		virtual void drawIndexedInstanced( uint32_t _numIndices, uint32_t _numInstances, uint32_t _baseVertex ) = 0;

///////////////////////////////////////////////////////////////////////////////////////
		
		cObjectHandleContainer<sProgram,      ProgramID>      m_programs;
		cObjectHandleContainer<sPipeline,     PipelineID>     m_pipelines;
		cObjectHandleContainer<sRenderTarget, RenderTargetID> m_renderTargets;
		cObjectHandleContainer<sGPUBuffer,    GPUBufferID>    m_gpuBuffers;
		cObjectHandleContainer<sTexture,      TextureID>      m_textures;
		cObjectHandleContainer<sMesh,         MeshID>         m_meshes;
		
///////////////////////////////////////////////////////////////////////////////////////

	protected:

		iLowLevelGraphics();

		std::thread::id m_threadID;

		virtual bool initialize( sLowLevelGraphicsDesc* _desc ) = 0;

		GraphicsAPI    m_graphicsApi = WV_GRAPHICS_API_NONE;
		GenericVersion m_graphicsApiVersion{};

		std::mutex m_mutex;
		
		cObjectHandleContainer<cCommandBuffer, CmdBufferID> m_commandBuffers;
		std::queue <CmdBufferID> m_availableCommandBuffers;
		std::vector<CmdBufferID> m_recordingCommandBuffers;
		std::vector<CmdBufferID> m_submittedCommandBuffers;

		GPUBufferID m_vertexBuffer{};

		cMaterial* m_pEmptyMaterial = nullptr;
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	inline void iLowLevelGraphics::cmd( CmdBufferID _bufferID, const eGPUTaskType& _type, T* _pInfo )
	{
		std::scoped_lock lock( m_mutex );
		m_commandBuffers.get( _bufferID ).push<T>( _type, _pInfo );
	}

	template<typename ID, typename T>
	inline ID iLowLevelGraphics::cmdCreateCommand( CmdBufferID _bufferID, eGPUTaskType _task, ID _id, const T& _desc )
	{
		sCmdCreateDesc<ID, T> desc{ _id, _desc };
		cmd( _bufferID, _task, &desc );
		return _id;
	}
}