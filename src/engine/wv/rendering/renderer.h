#pragma once


#include <wv/helpers/unordered_array.hpp>
#include <wv/math/vector2.h>
#include <wv/math/matrix.h>

#include <wv/rendering/command_buffer.h>
#include <wv/rendering/pipeline_manager.h>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

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

struct DescriptorAllocator
{
	struct PoolSizeRatio
	{
		VkDescriptorType type;
		float ratio;
	};

	void initialize( VkDevice _device, uint32_t _maxSets, std::span<PoolSizeRatio> _poolRatios ) {
		std::vector<VkDescriptorPoolSize> poolSizes;
		for ( PoolSizeRatio ratio : _poolRatios )
		{
			poolSizes.push_back(
				VkDescriptorPoolSize{
					.type = ratio.type,
					.descriptorCount = uint32_t( ratio.ratio * _maxSets )
				} );
		}

		VkDescriptorPoolCreateInfo poolInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		poolInfo.flags = 0;
		poolInfo.maxSets = _maxSets;
		poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();

		vkCreateDescriptorPool( _device, &poolInfo, nullptr, &pool );
	}

	void clear( VkDevice _device ) {
		vkResetDescriptorPool( _device, pool, 0 );
	}

	void destroy( VkDevice _device ) {
		vkDestroyDescriptorPool( _device, pool, nullptr );
	}

	VkDescriptorSet allocate( VkDevice _device, VkDescriptorSetLayout _layout ) {
		VkDescriptorSetAllocateInfo allocInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.descriptorPool = pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &_layout;

		VkDescriptorSet ds;
		vkAllocateDescriptorSets( _device, &allocInfo, &ds );

		return ds;
	}

	VkDescriptorPool pool;

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

	FrameData& getCurrentFrame() { return m_frames[ m_frameNumber % FRAME_OVERLAP ]; };

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
	
};


}