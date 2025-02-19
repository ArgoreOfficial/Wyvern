#pragma once

#include <wv/unordered_array.hpp>

#include <wv/Types.h>
#include <wv/Graphics/Types.h>

#include <wv/Misc/Color.h>
#include <wv/Graphics/GPUBuffer.h>

#include <wv/Graphics/CommandBuffer.h>
#include <wv/Graphics/Pipeline.h>
#include <wv/Graphics/DrawList.h>
#include <wv/Graphics/Texture.h>
#include <wv/Graphics/RenderTarget.h>

#include <vector>
#include <queue>
#include <deque>
#include <thread>
#include <mutex>
#include <shared_mutex>

#include <unordered_map>

///////////////////////////////////////////////////////////////////////////////////////

namespace wv {

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
struct sCmdCreateDesc { T id; D desc; };

struct CreateCallback
{
	void* caller = nullptr;
	wv::Function<void, void*>::fptr_t func{};
};

///////////////////////////////////////////////////////////////////////////////////////

class iLowLevelGraphics
{
public:

	virtual ~iLowLevelGraphics() { };

	static iLowLevelGraphics* createGraphics( sLowLevelGraphicsDesc* _desc );

	void initEmbeds();

	std::thread::id getThreadID() const { return m_threadID; }
	void assertMainThread() {
		if( std::this_thread::get_id() != getThreadID() )
	#ifdef __cpp_exceptions
			throw std::runtime_error( "Callstack not on render thread" );
	#else
		wv::Debug::Print( wv::Debug::WV_PRINT_ERROR, "Callstack not on render thread" );
	#endif
	}

	void executeCreateQueue();

private:
	template<typename T>
	void cmd( const eGPUTaskType& _type, T* _pInfo );
	void cmd( const eGPUTaskType& _type ) { cmd<char>( _type, nullptr ); }
public:

	template<typename ID, typename T>
	ID cmdCreateCommand( eGPUTaskType _task, ID _id, const T& _desc );

	ProgramID createProgram( const sProgramDesc& _desc );
	void      destroyProgram( ProgramID _programID );

	PipelineID createPipeline( const sPipelineDesc& _desc );
	void       destroyPipeline( PipelineID _pipelineID );

	RenderTargetID createRenderTarget( const sRenderTargetDesc& _desc );
	void           destroyRenderTarget( RenderTargetID _renderTargetID );

	GPUBufferID createGPUBuffer( const sGPUBufferDesc& _desc );
	void        destroyGPUBuffer( GPUBufferID _bufferID );

	MeshID createMesh( const sMeshDesc& _desc );
	void   destroyMesh( MeshID _meshID );

	TextureID createTexture( const sTextureDesc& _desc );
	void      destroyTexture( TextureID _textureID );
	void      bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps );
	virtual void      _bindTextureToSlot( TextureID _textureID, unsigned int _slot ) = 0;

	void cmdDrawIndexedIndirect( DrawListID _drawListID, sDrawIndexedIndirectCommand _cmd, const std::vector<sMeshInstanceData>& _instances );

	void queueAddCallback( wv::Function<void, void*>::fptr_t _func, void* _caller );

	cMaterial* getEmptyMaterial() { return m_pEmptyMaterial; }

///////////////////////////////////////////////////////////////////////////////////////

	virtual void terminate();

	//virtual void update() = 0;

	virtual void onResize( int _width, int _height ) = 0;
	virtual void setViewport( int _width, int _height ) = 0;

	virtual void beginRender();
	virtual void endRender();

	virtual void           setRenderTarget( RenderTargetID _renderTargetID ) = 0;

	virtual void setClearColor( const wv::cColor& _color ) = 0;
	virtual void clearRenderTarget( bool _color, bool _depth ) = 0;


	virtual void       bindPipeline( PipelineID _pipelineID ) = 0;


	virtual void bindBuffer( GPUBufferID _bufferID ) = 0;
	virtual void bindBufferIndex( GPUBufferID _bufferID, int32_t _bindingIndex ) = 0;

	virtual void bufferData( GPUBufferID _bufferID, void* _pData, size_t _size ) = 0;
	virtual void bufferSubData( GPUBufferID _bufferID, void* _pData, size_t _size, size_t _base ) = 0;

	virtual void copyBufferSubData( GPUBufferID _readBufferID, GPUBufferID _writeBufferID, size_t _readOffset, size_t _writeOffset, size_t _size ) = 0;


	virtual void bindVertexBuffer( GPUBufferID _vertexPullBufferID ) = 0;

	virtual void setFillMode( eFillMode _mode ) = 0;

	virtual void draw( uint32_t _firstVertex, uint32_t _numVertices ) = 0;
	virtual void drawIndexed( uint32_t _numIndices ) = 0;
	virtual void drawIndexedInstanced( uint32_t _numIndices, uint32_t _numInstances, uint32_t _baseVertex ) = 0;
	virtual void multiDrawIndirect( DrawListID _drawListID ) = 0;

///////////////////////////////////////////////////////////////////////////////////////

	wv::unordered_array<ProgramID, sProgram>      m_programs;
	wv::unordered_array<PipelineID, sPipeline>     m_pipelines;
	wv::unordered_array<RenderTargetID, sRenderTarget> m_renderTargets;
	wv::unordered_array<GPUBufferID, sGPUBuffer>    m_gpuBuffers;
	wv::unordered_array<TextureID, sTexture>      m_textures;
	wv::unordered_array<MeshID, sMesh>         m_meshes;

	std::queue<std::pair<ProgramID, sProgramDesc>> m_programCreateQueue;

	std::unordered_map<std::string, BufferBindingIndex> m_uniformBindingNameMap;

	wv::unordered_array<DrawListID, sDrawList> m_drawLists;
	std::unordered_map<PipelineID, DrawListID> m_pipelineDrawListMap;

	uint32_t drawIndirectHandle = 0;

///////////////////////////////////////////////////////////////////////////////////////

protected:
	virtual ProgramID _createProgram( ProgramID _programID, const sProgramDesc& _desc ) = 0;
	virtual void      _destroyProgram( ProgramID _programID ) = 0;

	virtual GPUBufferID _createGPUBuffer( GPUBufferID _bufferID, const sGPUBufferDesc& _desc ) = 0;
	virtual void        _destroyGPUBuffer( GPUBufferID _bufferID ) = 0;

	virtual RenderTargetID _createRenderTarget( RenderTargetID _renderTargetID, const sRenderTargetDesc& _desc ) = 0;
	virtual void           _destroyRenderTarget( RenderTargetID _renderTargetID ) = 0;

	virtual PipelineID _createPipeline( PipelineID _pipelineID, const sPipelineDesc& _desc ) = 0;
	virtual void       _destroyPipeline( PipelineID _pipelineID ) = 0;

	virtual MeshID _createMesh( MeshID _meshID, const sMeshDesc& _desc );
	virtual void   _destroyMesh( MeshID _meshID );

	virtual TextureID _createTexture( TextureID _textureID, const sTextureDesc& _desc ) = 0;
	virtual void      _destroyTexture( TextureID _textureID ) = 0;

	virtual void      _bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps ) = 0;


	iLowLevelGraphics();

	// returns base vertex
	size_t pushVertexBuffer( void* _vertices, size_t _size );
	// returns base index
	size_t pushIndexBuffer( void* _indices, size_t _size );

	virtual bool initialize( sLowLevelGraphicsDesc* _desc ) = 0;

	std::thread::id m_threadID;

	GraphicsAPI    m_graphicsApi = WV_GRAPHICS_API_NONE;
	GenericVersion m_graphicsApiVersion{};

	std::mutex m_mutex;

	cCommandBuffer m_createDestroyCommandBuffer{ 128 };

	GPUBufferID m_vertexBuffer{};
	GPUBufferID m_indexBuffer{};

	cMaterial* m_pEmptyMaterial = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void iLowLevelGraphics::cmd( const eGPUTaskType& _type, T* _pInfo )
{
	std::scoped_lock lock( m_mutex );
	m_createDestroyCommandBuffer.push<T>( _type, _pInfo );
}

template<typename ID, typename T>
inline ID iLowLevelGraphics::cmdCreateCommand( eGPUTaskType _task, ID _id, const T& _desc )
{
	sCmdCreateDesc<ID, T> desc{ _id, _desc };
	cmd( _task, &desc );
	return _id;
}
}