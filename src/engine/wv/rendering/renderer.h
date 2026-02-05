#pragma once


#include <wv/debug/log.h>

#include <wv/helpers/unordered_array.hpp>
#include <wv/math/vector2.h>
#include <wv/math/matrix.h>
#include <wv/math/vector3.h>
#include <wv/math/matrix.h>

#include <wv/rendering/command_buffer.h>
#include <wv/rendering/pipeline_manager.h>
#include <wv/rendering/image_manager.h>

#include <wv/resource_id.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <functional>
#include <stdint.h>
#include <vector>
#include <span>
#include <set>

namespace wv {

class World;
class Swapchain;

struct SceneData
{
	wv::Matrix4x4f viewProj;
};

class RingFences
{
public:
	void initalize( VkDevice _device, uint32_t _cycleSize = 2 ) 
	{
		m_device = _device;
		m_cycleSize = _cycleSize;
		m_fences.resize( _cycleSize );

		VkFenceCreateInfo fenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for ( uint32_t i = 0; i < _cycleSize; i++ )
			vkCreateFence( _device, &fenceCreateInfo, nullptr, &m_fences[ i ] );
	}

	void shutdown() {
		for ( uint32_t i = 0; i < m_cycleSize; i++ )
			vkDestroyFence( m_device, m_fences[ i ], nullptr);
	}

	// waits and resets fence
	void setCycle( uint32_t _cycle ) {
		m_cycleIndex = _cycle % m_cycleSize;

		vkWaitForFences( m_device, 1, &m_fences[ m_cycleIndex ], true, 1000000000);
		vkResetFences( m_device, 1, &m_fences[ m_cycleIndex ] );
	}

	VkFence getFence() const { return m_fences[ m_cycleIndex ]; }

private:
	VkDevice m_device{ VK_NULL_HANDLE };

	uint32_t m_cycleSize{ 0 };
	uint32_t m_cycleIndex{ 0 };
	
	std::vector<VkFence> m_fences{};
};

struct FrameData
{
	VkCommandPool commandPool = VK_NULL_HANDLE;
	CommandBuffer* mainCommandBuffer = nullptr;

	VkSemaphore acquireSemaphore = VK_NULL_HANDLE;
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

class Renderer
{
	friend class Application;
	friend class PipelineManager;
	friend class ImageManager;

public:
	bool initialize();
	void shutdown();

	void prepare( uint32_t _width, uint32_t _height );
	void render( World* _world );
	void finalize();

	bool isSwapchainOutOfDate() const { return m_resizeRequested; }

	ResourceID createPipeline( uint32_t* _vertSrc, uint32_t _vertSize, uint32_t* _fragSrc, uint32_t _fragSize );
	void destroyPipeline( ResourceID _pipeline );

	ResourceID allocateMesh( const std::vector<uint16_t>& _indices, const std::vector<Vector3f>& _vertexPositions, void* _vertexData = nullptr, size_t _vertexDataSize = 0 );
	void deallocateMesh( ResourceID _mesh );

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

protected:
	void waitForRenderer() const { vkDeviceWaitIdle( m_device ); }
	
	FrameData& getCurrentFrame() { return m_frames[ m_frameNumber % FRAME_OVERLAP ]; };

	bool initVulkan();
	bool initSwapchain( uint32_t _width, uint32_t _height );
	bool initCommands();
	bool initSyncStructures();
	bool initDescriptors();

	void resizeSwapchain( uint32_t _width, uint32_t _height );
	
	void drawBackground( CommandBuffer* _cmd );
	void drawGeometry( CommandBuffer* _cmd, World* _world );

	void immediateCmdSubmit( std::function<void( CommandBuffer& _cmd )>&& _func );

	AllocatedBuffer createBuffer( size_t _size, VkBufferUsageFlags _usage, VmaMemoryUsage _memoryUsage );
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

	uint32_t  m_frameNumber = 0;
	FrameData m_frames[ FRAME_OVERLAP ];
	RingFences m_ringFences{};

	VkQueue  m_graphicsQueue       = VK_NULL_HANDLE;
	uint32_t m_graphicsQueueFamily = 0;
	
	VmaAllocator m_allocator = VK_NULL_HANDLE;

	VkFence        m_immediateFence         = VK_NULL_HANDLE;
	VkCommandPool  m_immediateCommandPool   = VK_NULL_HANDLE;
	CommandBuffer* m_immediateCommandBuffer = nullptr;

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

};


}