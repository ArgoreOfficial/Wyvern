#pragma once


#include <wv/debug/log.h>

#include <wv/helpers/unordered_array.hpp>
#include <wv/math/vector2.h>
#include <wv/math/matrix.h>
#include <wv/math/vector3.h>
#include <wv/math/matrix.h>

#include <wv/pool.h>

#include <wv/rendering/command_buffer.h>
#include <wv/rendering/pipeline_manager.h>
#include <wv/rendering/image_manager.h>

#include <wv/resource_id.h>

#include <wv/rendering/vulkan.h>
#include <vk_mem_alloc.h>

#include <functional>
#include <stdint.h>
#include <vector>
#include <span>
#include <set>
#include <utility>

namespace wv {

class World;
class Swapchain;

struct SceneData
{
	wv::Matrix4x4f viewProj;
};

struct DeleteQueue
{
	std::vector<std::function<void()>> deleteQueue;

	void push( const std::function<void()>& _func ) { deleteQueue.push_back( _func ); }

	void flush() {
		for ( auto it = deleteQueue.rbegin(); it != deleteQueue.rend(); it++ )
			( *it )( );
		deleteQueue.clear();
	}
};

constexpr uint32_t FRAME_OVERLAP = 2;

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	VmaAllocationInfo info;

	VkDeviceSize actualSize = 0;
	VkDeviceAddress deviceAddress = 0;
};

struct MeshAllocation
{
	AllocatedBuffer indexBuffer;
	AllocatedBuffer positionBuffer;
	AllocatedBuffer vertexDataBuffer;

	VkDeviceAddress positionBufferAddress;
	VkDeviceAddress vertexDataBufferAddress;
};

struct GPUDrawPushConstants
{
	wv::Matrix4x4f viewProj;
	wv::Matrix4x4f model;
	VkDeviceAddress positionBuffer;
	VkDeviceAddress vertexDataBuffer;
};

enum class SamplerState
{
	WV_SAMPLER_LINEAR,
	WV_SAMPLER_NEAREST
};

struct VirtualAllocSpan
{
	VmaVirtualAllocation alloc;
	VkDeviceSize offset;
	VkDeviceSize size;

	void* mapping;
	VkBuffer buffer;

	size_t virtualBlockIndex = 0;
};

class StagingBufferRing
{
public:
	void initialize( VmaAllocator _allocator, VkDeviceSize _initialSize, uint32_t _cycleSize ) {
		m_cycleSize = _cycleSize;
		m_stagingSpanRing.resize( _cycleSize );
		m_allocator = _allocator;
		allocateStaging( _initialSize );
	}

	void shutdown() {
		for ( size_t i = 0; i < m_cycleSize; i++ )
			clearAllocations( i );
		
		for ( StagingBufferAllocation& buffer : m_stagingBuffers )
		{
			vmaDestroyBuffer( m_allocator, buffer.buffer, buffer.allocation );
			vmaDestroyVirtualBlock( buffer.virtualBlock );
		}
	}

	void setCycle( uint32_t _cycle ) {
		m_cycleIndex = _cycle % m_cycleSize;
		clearAllocations( m_cycleIndex );
	}

	void clearAllocations( uint32_t _cycle ) {
		for ( VirtualAllocSpan span : getAllocVec( _cycle ) )
			vmaVirtualFree( m_stagingBuffers[ span.virtualBlockIndex ].virtualBlock, span.alloc);

		getAllocVec( _cycle ).clear();
	}

	VirtualAllocSpan allocate( VkDeviceSize _size );

private:
	struct StagingBufferAllocation
	{
		VkBuffer buffer;
		VmaAllocation allocation;
		VmaAllocationInfo info;

		VmaVirtualBlock virtualBlock; // unused
	};

	typedef std::vector<VirtualAllocSpan> virtual_alloc_vec;

	StagingBufferAllocation allocateStaging( VkDeviceSize _size ) {
		// allocate buffer

		VkBufferCreateInfo bufferInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferInfo.size = wv::Math::align<VkDeviceSize>( _size, 4096 );
		bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		StagingBufferAllocation buffer{};
		vmaCreateBuffer( m_allocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, &buffer.info );

		// allocate block

		VmaVirtualBlockCreateInfo blockInfo{};
		blockInfo.size = _size;

		VkResult res = vmaCreateVirtualBlock( &blockInfo, &buffer.virtualBlock );

		WV_ASSERT( res == VK_SUCCESS );

		m_stagingBuffers.push_back( buffer );
		return buffer;
	}

	VkResult tryAllocateSpan( VkDeviceSize _size, size_t _blockIndex, VirtualAllocSpan& _outSpan );

	virtual_alloc_vec& getAllocVec( uint32_t _cycle ) {
		return m_stagingSpanRing[ _cycle % m_cycleSize ];
	}

	std::mutex m_mtx;
	VmaAllocator m_allocator{ VK_NULL_HANDLE };

	uint32_t m_cycleSize{ 0 };
	uint32_t m_cycleIndex{ 0 };

	std::vector<virtual_alloc_vec> m_stagingSpanRing;
	std::vector<StagingBufferAllocation> m_stagingBuffers;
};

class Renderer
{
	friend class Application;
	friend class PipelineManager;
	friend class ImageManager;

public:
	bool initialize();
	void shutdown();

	void render( World* _world );

	bool isSwapchainOutOfDate() const { return m_resizeRequested; }

	ResourceID createPipeline( uint32_t* _vertSrc, uint32_t _vertSize, uint32_t* _fragSrc, uint32_t _fragSize, TopologyClass _topology = TopologyClass::WV_TRIANGLE );
	void destroyPipeline( ResourceID _pipeline );

	ResourceID allocateMesh( uint32_t _numIndices, uint32_t _numPositions, uint32_t _vertexDataSize = 0 );
	void deallocateMesh( ResourceID _mesh );
	void uploadMesh( ResourceID _mesh, const uint32_t* _indices, const Vector3f* _positions, const void* _vertexData = nullptr );

	ResourceID allocateImage( const void* _data, int _width, int _height, bool _mipmapped );
	void deallocateImage( ResourceID _image );

	uint32_t storeImage( ResourceID _imageID, SamplerState _filter ) {
		std::scoped_lock lock{ m_mtx };

		VkSampler sampler = VK_NULL_HANDLE;
		switch ( _filter )
		{
		case SamplerState::WV_SAMPLER_LINEAR:  sampler = m_samplerLinear;  break;
		case SamplerState::WV_SAMPLER_NEAREST: sampler = m_samplerNearest; break;
		}

		uint32_t index = 0;
		while ( m_storedImageIndices.contains( index ) )
			index++;

		storeImage( _imageID, sampler, index );
		return index;
	}

	void addDebugLine( const wv::Vector3f& _start, const wv::Vector3f& _end ) {
		m_debugLinePositions.push_back( _start );
		m_debugLinePositions.push_back( _end );
	}

	void addDebugSphere( const wv::Vector3f& _centre, float _size );

protected:

	static VkBool32 debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT _messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData,
		void* _pUserData );

	void waitForRenderer() const { vkDeviceWaitIdle( m_device ); }
	
	bool initVulkan();
	bool initSwapchain( uint32_t _width, uint32_t _height );
	bool initSyncStructures();
	bool initDescriptors();

	void resizeSwapchain( uint32_t _width, uint32_t _height );
	
	void beginDebugRender();
	void endDebugRender();

	void drawBackground( VkCommandBuffer _cmd );
	void drawGeometry( VkCommandBuffer _cmd, World* _world );
	void drawDebug( VkCommandBuffer _cmd, World* _world );

	void immediateCmdSubmit( std::function<void( VkCommandBuffer _cmd )>&& _func );

	AllocatedBuffer createBuffer( size_t _size, VkBufferUsageFlags _usage, VmaMemoryUsage _memoryUsage, const char* _debugName = nullptr );
	void destroyBuffer( const AllocatedBuffer& _buffer );

	void storeImage( ResourceID _imageID, VkSampler _sampler, uint32_t _at );
	
	const bool m_useValidationLayers = true;

	bool m_initialized = false;
	bool m_resizeRequested = false;
	
	std::recursive_mutex m_mtx = {};

	PipelineManager m_pipelineManager = { this };
	ImageManager    m_imageManager    = { this };

	DeleteQueue m_mainDeleteQueue = {};

	VkInstance               m_instance       = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	VkPhysicalDevice         m_physicalDevice = VK_NULL_HANDLE;
	VkDevice                 m_device         = VK_NULL_HANDLE;
	VkSurfaceKHR             m_surface        = VK_NULL_HANDLE;

	VkPhysicalDeviceLimits   m_deviceLimits = {};

	Swapchain* m_swapchain = nullptr;

	ResourceID m_drawImage  = {};
	ResourceID m_depthImage = {};
	VkExtent2D m_drawExtent = {};

	uint32_t m_frameNumber = 0;
	
	FenceRing       m_fenceRing{};
	SemaphoreRing   m_semaphoreRing{};
	CommandPoolRing m_commandPoolRing{};

	GenericRingPool<DeleteQueue> m_deleteQueueRing{};
	// for transient fences
	FencePool m_fencePool{};

	VkQueue  m_graphicsQueue       = VK_NULL_HANDLE;
	uint32_t m_graphicsQueueFamily = 0;
	
	VmaAllocator m_allocator = VK_NULL_HANDLE;

	StagingBufferRing m_stagingRing = {};

	typedef std::function<void()> loadCallback_t;
	std::vector<std::pair<VkFence, loadCallback_t>> m_loadCallbacks{};

	// Bindless

	const uint32_t STORAGE_BINDING = 0;
	const uint32_t SAMPLER_BINDING = 1;
	const uint32_t IMAGE_BINDING   = 2;
	
	const uint32_t STORAGE_COUNT = 65536;
	const uint32_t SAMPLER_COUNT = 65536;
	const uint32_t IMAGE_COUNT   = 65536;

	VkDescriptorPool      m_bindlessPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout m_bindlessLayout = VK_NULL_HANDLE;
	VkDescriptorSet       m_bindlessDescriptorSet = VK_NULL_HANDLE;
	VkPipelineLayout      m_bindlessPipelineLayout = VK_NULL_HANDLE;

	// Debug Images
	
	VkSampler m_samplerLinear  = VK_NULL_HANDLE;
	VkSampler m_samplerNearest = VK_NULL_HANDLE;
	
	std::set<uint32_t> m_storedImageIndices = { };
	std::unordered_map<ResourceID, uint32_t> m_storedImageIndexMap;

	ResourceID m_blackImage{};
	ResourceID m_whiteImage{};
	ResourceID m_debugImage{};
	
	unordered_array<ResourceID, MeshAllocation> m_meshAllocations;

	// Debug drawing

	GenericRingPool<AllocatedBuffer> m_debugLineBuffers{};
	std::vector<wv::Vector3f> m_debugLinePositions{ };

	// Debug error 

	std::vector<std::pair<int32_t, std::string>> m_errorInfos;
	void pushErrorInfo( int32_t _messageID, const std::string& _message ) { m_errorInfos.emplace_back( _messageID, _message ); }
	void popErrorInfo() { m_errorInfos.pop_back(); }
};


}