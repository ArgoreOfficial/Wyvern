#pragma once

#include <stdint.h>

#include <wv/helpers/unordered_array.hpp>
#include <wv/math/vector2.h>
#include <wv/math/matrix.h>

#include <vulkan/vulkan.h>

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
	VkCommandBuffer mainCommandBuffer;

	VkSemaphore acquireSemaphore;
	VkFence fence;
};

constexpr uint32_t FRAME_OVERLAP = 2;

class Renderer
{
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

	void createSwapchain( uint32_t _width, uint32_t _height );
	void destroySwapchain();

	FrameData& getCurrentFrame() { return m_frames[ m_frameNumber % FRAME_OVERLAP ]; };

	const bool m_useValidationLayers = true;

	bool m_initialized = false;

	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	VkSurfaceKHR m_surface;

	VkSwapchainKHR m_swapchain;
	VkFormat m_swapchainImageFormat;

	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;
	VkExtent2D m_swapchainExtent;

	uint32_t m_frameNumber;
	FrameData m_frames[ FRAME_OVERLAP ];
	std::vector<VkSemaphore> m_submitSemaphores;

	VkQueue m_graphicsQueue;
	uint32_t m_graphicsQueueFamily;
};


}