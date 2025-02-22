#pragma once

#include <wv/unordered_array.hpp>

#include <wv/types.h>
#include <wv/graphics/types.h>

#include <wv/misc/color.h>
#include <wv/graphics/gpu_buffer.h>

#include <wv/graphics/cmdbuffer.h>
#include <wv/graphics/pipeline.h>
#include <wv/graphics/draw_list.h>
#include <wv/graphics/texture.h>
#include <wv/graphics/render_target.h>

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

class IDeviceContext;
class ShaderResource;
class Material;

///////////////////////////////////////////////////////////////////////////////////////

struct LowLevelGraphicsDesc
{
	GraphicsDriverLoadProc loadProc;
	IDeviceContext* pContext;
};

template<typename T, typename D>
struct CmdCreateDesc { T id; D desc; };

struct CreateCallback
{
	void* caller = nullptr;
	wv::Function<void, void*>::fptr_t func{};
};

///////////////////////////////////////////////////////////////////////////////////////

class IGraphicsDevice
{
public:

	virtual ~IGraphicsDevice() { };

	static IGraphicsDevice* createGraphics( LowLevelGraphicsDesc* _desc );

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
	void cmd( const GPUTaskType& _type, T* _pInfo );
	void cmd( const GPUTaskType& _type ) { cmd<char>( _type, nullptr ); }

public:

	template<typename ID, typename T>
	ID cmdCreateCommand( GPUTaskType _task, ID _id, const T& _desc );

	ShaderModuleID createShaderModule( const ShaderModuleDesc& _desc );
	void      destroyShaderModule( ShaderModuleID _programID );

	PipelineID createPipeline( const PipelineDesc& _desc );
	void       destroyPipeline( PipelineID _pipelineID );

	RenderTargetID createRenderTarget( const RenderTargetDesc& _desc );
	void           destroyRenderTarget( RenderTargetID _renderTargetID );

	GPUBufferID createGPUBuffer( const GPUBufferDesc& _desc );
	void        destroyGPUBuffer( GPUBufferID _bufferID );

	MeshID createMesh( const MeshDesc& _desc );
	void   destroyMesh( MeshID _meshID );

	TextureID    createTexture( const TextureDesc& _desc );
	void         destroyTexture( TextureID _textureID );
	void         bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps );
	virtual void bindTextureToSlot( TextureID _textureID, unsigned int _slot ) = 0;

	void queueAddCallback( wv::Function<void, void*>::fptr_t _func, void* _caller );

	Material* getEmptyMaterial() { return m_pEmptyMaterial; }

///////////////////////////////////////////////////////////////////////////////////////

	virtual void terminate();

	virtual void beginRender();
	virtual void endRender();

	// cmd

	virtual void cmdBeginRender( 
		CmdBufferID _cmd, 
		RenderTargetID _renderTargetID ) = 0;

	virtual void cmdEndRender( 
		CmdBufferID _cmd ) = 0;
	
	virtual void cmdClearColor( 
		CmdBufferID _cmd, 
		float _r, 
		float _g, 
		float _b, 
		float _a ) = 0;

	virtual void cmdImageClearColor( 
		CmdBufferID _cmd, 
		TextureID _image, 
		float _r, 
		float _g, 
		float _b,
		float _a ) = 0;

	virtual void cmdClearDepthStencil( 
		CmdBufferID _cmd, 
		double _depth, 
		uint32_t _stencil ) = 0;

	virtual void cmdImageClearDepthStencil( 
		CmdBufferID _cmd, 
		TextureID _image, 
		double _depth, 
		uint32_t _stencil ) = 0;

	virtual void cmdBindPipeline( 
		CmdBufferID _cmd, 
		PipelineID _pipeline ) = 0;

	virtual void cmdImageBlit( 
		CmdBufferID _cmd, 
		TextureID _src, 
		TextureID _dst ) = 0;

	virtual void cmdDispatch( 
		CmdBufferID _cmd, 
		uint32_t _numGroupsX, 
		uint32_t _numGroupsY, 
		uint32_t _numGroupsZ ) = 0;

	virtual void cmdViewport( 
		CmdBufferID _cmd, 
		uint32_t _x, 
		uint32_t _y, 
		uint32_t _width, 
		uint32_t _height ) = 0;

	virtual void cmdCopyBuffer( 
		CmdBufferID _cmd, 
		GPUBufferID _src, 
		GPUBufferID _dst, 
		size_t _srcOffset, 
		size_t _dstOffset, 
		size_t _size ) = 0;
	
	virtual void cmdBindVertexBuffer( 
		CmdBufferID _cmd, 
		GPUBufferID _vertexBuffer ) = 0;

	virtual void cmdBindIndexBuffer( 
		CmdBufferID _cmd, 
		GPUBufferID _indexBuffer, 
		size_t _offset, 
		wv::DataType _type ) = 0;

	virtual void cmdUpdateBuffer( 
		CmdBufferID _cmd, 
		GPUBufferID _buffer,
		size_t _dataSize,
		void* _pData ) = 0;

	virtual void cmdUpdateSubBuffer(
		CmdBufferID _cmd,
		GPUBufferID _buffer,
		size_t _offset,
		size_t _dataSize,
		void* _pData ) = 0;

	virtual void cmdDraw( 
		CmdBufferID _cmd, 
		uint32_t _vertexCount, 
		uint32_t _instanceCount, 
		uint32_t _firstVertex, 
		uint32_t _firstInstance ) = 0;

	virtual void cmdDrawIndexed( 
		CmdBufferID _cmd, 
		uint32_t _indexCount, 
		uint32_t _instanceCount, 
		uint32_t _firstIndex, 
		int32_t _vertexOffset, 
		uint32_t _firstInstance ) = 0;
	
	// old 

	virtual void bindBufferIndex( 
		GPUBufferID _bufferID, 
		int32_t _bindingIndex ) = 0;

	virtual void bufferSubData( 
		GPUBufferID _bufferID, 
		void* _pData, 
		size_t _size, 
		size_t _base ) = 0;

	virtual void setFillMode( 
		FillMode _mode ) = 0;

///////////////////////////////////////////////////////////////////////////////////////

	wv::unordered_array<ShaderModuleID, Program>      m_programs;
	wv::unordered_array<PipelineID,     Pipeline>     m_pipelines;
	wv::unordered_array<RenderTargetID, RenderTarget> m_renderTargets;
	wv::unordered_array<GPUBufferID,    GPUBuffer>    m_gpuBuffers;
	wv::unordered_array<TextureID,      Texture>      m_textures;
	wv::unordered_array<MeshID,         Mesh>         m_meshes;

	std::queue<std::pair<ShaderModuleID, ShaderModuleDesc>> m_programCreateQueue;

	std::unordered_map<std::string, BufferBindingIndex> m_uniformBindingNameMap;

	uint32_t drawIndirectHandle = 0;

///////////////////////////////////////////////////////////////////////////////////////

protected:
	virtual ShaderModuleID _createShaderModule( ShaderModuleID _programID, const ShaderModuleDesc& _desc ) = 0;
	virtual void      _destroyShaderModule( ShaderModuleID _programID ) = 0;

	virtual GPUBufferID _createGPUBuffer( GPUBufferID _bufferID, const GPUBufferDesc& _desc ) = 0;
	virtual void        _destroyGPUBuffer( GPUBufferID _bufferID ) = 0;

	virtual RenderTargetID _createRenderTarget( RenderTargetID _renderTargetID, const RenderTargetDesc& _desc ) = 0;
	virtual void           _destroyRenderTarget( RenderTargetID _renderTargetID ) = 0;

	virtual PipelineID _createPipeline( PipelineID _pipelineID, const PipelineDesc& _desc ) = 0;
	virtual void       _destroyPipeline( PipelineID _pipelineID ) = 0;

	virtual MeshID _createMesh( MeshID _meshID, const MeshDesc& _desc );
	virtual void   _destroyMesh( MeshID _meshID );

	virtual TextureID _createTexture( TextureID _textureID, const TextureDesc& _desc ) = 0;
	virtual void      _destroyTexture( TextureID _textureID ) = 0;

	virtual void _bufferTextureData( TextureID _textureID, void* _pData, bool _generateMipMaps ) = 0;

	IGraphicsDevice();

	virtual bool initialize( LowLevelGraphicsDesc* _desc ) = 0;

	std::thread::id m_threadID;

	GraphicsAPI    m_graphicsApi = WV_GRAPHICS_API_NONE;
	GenericVersion m_graphicsApiVersion{};

	std::mutex m_mutex;

	CommandBuffer m_createDestroyCommandBuffer{ 128 };

	Material* m_pEmptyMaterial = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////

template<typename T>
inline void IGraphicsDevice::cmd( const GPUTaskType& _type, T* _pInfo )
{
	std::scoped_lock lock{ m_mutex };
	m_createDestroyCommandBuffer.push<T>( _type, _pInfo );
}

template<typename ID, typename T>
inline ID IGraphicsDevice::cmdCreateCommand( GPUTaskType _task, ID _id, const T& _desc )
{
	CmdCreateDesc<ID, T> desc{ _id, _desc };
	cmd( _task, &desc );
	return _id;
}
}