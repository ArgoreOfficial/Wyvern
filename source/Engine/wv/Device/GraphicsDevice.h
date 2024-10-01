#pragma once

#include <wv/Types.h>

#include <wv/Shader/Shader.h>
#include <wv/Shader/ShaderProgram.h>

#include <wv/Misc/Color.h>
#include <wv/Graphics/GPUBuffer.h>

#include <wv/Graphics/CommandBuffer.h>

#include <vector>
#include <set>
#include <queue>
#include <thread>
#include <mutex>
#include <shared_mutex>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv
{

///////////////////////////////////////////////////////////////////////////////////////

	class RenderTarget;
	class cMaterial;

	struct PipelineDesc;

	struct sMeshDesc;
	struct sMeshNode;
	struct sMesh;
	
	struct sTextureDesc;
	struct sTexture;

	struct RenderTargetDesc;

	struct iDeviceContext;

	struct sPipelineDesc;
	struct sPipeline;

	struct sGPUBufferDesc;

	
///////////////////////////////////////////////////////////////////////////////////////

	struct GraphicsDeviceDesc
	{
		GraphicsDriverLoadProc loadProc;
		iDeviceContext* pContext;
	};

///////////////////////////////////////////////////////////////////////////////////////

	template<typename T, typename H>
	class cObjectContainer
	{
	public:
		cObjectContainer() 
		{
			m_pObjectBuffer = new uint8_t[ sizeof( T ) ];
			m_size = sizeof( T );
		}

		H allocate() 
		{
			uint64_t idx = 1;
			while( m_ids.contains( (H)idx ) )
				idx++;

			if( idx * sizeof( T ) >= m_size )
			{
				realloc( m_pObjectBuffer, m_size + sizeof( T ) );
				m_size += sizeof( T );
			}


			uint8_t* base = m_pObjectBuffer + sizeof( T ) * idx;
			T* obj = new( base ) T();

			m_ids.insert( (H)idx );

			return (H)idx;
		}

		void deallocate() 
		{ 
			/* do deallocation here */ 
		}

		T& get( H _id )
		{
			uint8_t* base = m_pObjectBuffer + sizeof( T ) * (uint64_t)_id;
			return *(T*)base;
		}

	private:
		
		std::set<H> m_ids;
		uint8_t* m_pObjectBuffer = nullptr;
		size_t m_size = 0;

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

		virtual RenderTarget* createRenderTarget( RenderTargetDesc* _desc ) = 0;
		virtual void destroyRenderTarget( RenderTarget** _renderTarget ) = 0;

		virtual void setRenderTarget( RenderTarget* _target ) = 0;
		virtual void setClearColor( const wv::cColor& _color ) = 0;
		virtual void clearRenderTarget( bool _color, bool _depth ) = 0;

		virtual ShaderProgramID createProgram( sShaderProgramDesc* _desc ) = 0;
		virtual void destroyProgram( ShaderProgramID _pProgram ) = 0;

		virtual sPipeline* createPipeline ( sPipelineDesc* _desc ) = 0;
		virtual void       destroyPipeline( sPipeline* _pPipeline ) = 0;
		virtual void       bindPipeline   ( sPipeline* _pPipeline ) = 0;

		virtual cGPUBuffer* createGPUBuffer ( sGPUBufferDesc* _desc ) = 0;
		virtual void        allocateBuffer  ( cGPUBuffer* _buffer, size_t _size ) = 0;
		virtual void        bufferData      ( cGPUBuffer* _buffer ) = 0;
		virtual void        destroyGPUBuffer( cGPUBuffer* _buffer ) = 0;
		
		virtual sMesh* createMesh( sMeshDesc* _desc ) = 0;
		virtual void   destroyMesh( sMesh* _pMesh ) = 0;

		virtual sTexture createTexture    ( sTextureDesc* _pDesc )                    = 0;
		virtual void     bufferTextureData( sTexture* _pTexture, void* _pData, bool _generateMipMaps ) = 0;
		virtual void     destroyTexture   ( sTexture* _pTexture )                     = 0;
		virtual void     bindTextureToSlot( sTexture* _pTexture, unsigned int _slot ) = 0;

		virtual void bindVertexBuffer( sMesh* _pMesh, cProgramPipeline* _pPipeline ) = 0;

		virtual void setFillMode( eFillMode _mode ) = 0;

		virtual void draw( sMesh* _pMesh ) = 0;
		virtual void drawIndexed( uint32_t _numIndices ) = 0;
		virtual void drawIndexedInstances( uint32_t _numIndices, uint32_t _numInstances ) = 0;

///////////////////////////////////////////////////////////////////////////////////////
		
		//std::unordered_map<ShaderProgramID, sShaderProgram*> m_shaderPrograms;
		cObjectContainer<sShaderProgram, ShaderProgramID> m_shaderPrograms2;

		ShaderProgramID allocateShaderProgramID();
		void deallocateShaderProgramID( ShaderProgramID _programID );


	protected:

		iGraphicsDevice() { };

		std::thread::id m_threadID;

		virtual bool initialize( GraphicsDeviceDesc* _desc ) = 0;

		GraphicsAPI    m_graphicsApi = WV_GRAPHICS_API_NONE;
		GenericVersion m_graphicsApiVersion {};

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