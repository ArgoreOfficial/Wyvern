#pragma once


#include <wv/helpers/unordered_array.hpp>
#include <wv/math/vector2.h>
#include <wv/math/matrix.h>

#include <wv/rendering/command_buffer.h>
#include <wv/rendering/pipeline_manager.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <wv/math/vector3.h>
#include <wv/math/matrix.h>

#include <functional>
#include <stdint.h>
#include <vector>
#include <span>

namespace wv {

class World;

struct SceneData
{
	wv::Matrix4x4f viewProj;
};

struct MaterialData
{
	wv::Matrix4x4f model;
};

struct FrameData
{
	VkCommandPool commandPool;
	CommandBuffer* mainCommandBuffer = nullptr;

	VkSemaphore acquireSemaphore;
	VkFence fence;
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

struct AllocatedImage
{
	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	VkExtent3D imageExtent;
	VkFormat imageFormat;
};

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	VmaAllocationInfo info;
};

struct Vertex
{
	wv::Vector3f position;
};

struct GPUMeshBuffers
{
	AllocatedBuffer indexBuffer;
	AllocatedBuffer vertexBuffer;
	VkDeviceAddress vertexBufferAddress;
};

struct GPUDrawPushConstants
{
	wv::Matrix4x4f worldMatrix;
	VkDeviceAddress vertexBuffer;
};

class Renderer
{
	friend class PipelineManager;

public:
	bool initialize();
	void shutdown();

	void prepare( uint32_t _width, uint32_t _height );
	void render( World* _world );
	void finalize();

protected:

	FrameData& getCurrentFrame() { return m_frames[ m_frameNumber % FRAME_OVERLAP ]; };

	bool initVulkan();
	bool initSwapchain( uint32_t _width, uint32_t _height );
	bool initCommands();
	bool initSyncStructures();
	bool initDescriptors();

	void createSwapchain( uint32_t _width, uint32_t _height );
	void destroySwapchain();

	void drawBackground( CommandBuffer* _cmd );
	void drawGeometry( CommandBuffer* _cmd );

	void immediateCmdSubmit( std::function<void( CommandBuffer& _cmd )>&& _func );

	AllocatedBuffer createBuffer( size_t _size, VkBufferUsageFlags _usage, VmaMemoryUsage _memoryUsage );
	void destroyBuffer( const AllocatedBuffer& _buffer );

	GPUMeshBuffers uploadMesh( const std::vector<uint16_t>& _indices, const std::vector<Vector3f>& _vertexPositions );
	void destroyMesh( const GPUMeshBuffers& _mesh );

	const bool m_useValidationLayers = true;

	bool m_initialized = false;

	PipelineManager m_pipelineManager = { this };

	DeleteQueue m_mainDeleteQueue = {};

	VkInstance               m_instance       = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;
	VkPhysicalDevice         m_physicalDevice = VK_NULL_HANDLE;
	VkDevice                 m_device         = VK_NULL_HANDLE;
	VkSurfaceKHR             m_surface        = VK_NULL_HANDLE;

	VkSwapchainKHR m_swapchain            = VK_NULL_HANDLE;
	VkFormat       m_swapchainImageFormat = VK_FORMAT_UNDEFINED;

	std::vector<VkImage>     m_swapchainImages     = {};
	std::vector<VkImageView> m_swapchainImageViews = {};
	std::vector<VkSemaphore> m_submitSemaphores    = {};
	VkExtent2D m_swapchainExtent = {};

	AllocatedImage m_drawImage  = {};
	VkExtent2D     m_drawExtent = {};

	uint32_t  m_frameNumber = 0;
	FrameData m_frames[ FRAME_OVERLAP ];

	VkQueue  m_graphicsQueue = VK_NULL_HANDLE;
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

	VkDescriptorPool      m_bindlessPool;
	VkDescriptorSetLayout m_bindlessLayout;
	VkDescriptorSet       m_bindlessDescriptorSet;
	VkPipelineLayout      m_bindlessPipelineLayout;

	// TESTING STUFF

	PipelineID m_trianglePipelineID = {};
	GPUMeshBuffers m_triangleBuffers = {};
};


}