#pragma once

#include <stdint.h>

#include <wv/helpers/unordered_array.hpp>
#include <wv/math/vector2.h>
#include <wv/math/matrix.h>

#include <vulkan/vulkan.h>

namespace wv {

class Entity;
class World;

struct SceneData
{
	wv::Matrix4x4f viewProj;
};

struct MaterialData
{
	wv::Matrix4x4f model;
};

class Renderer
{
public:
	bool initialize();
	void shutdown();

	void prepare( uint32_t _width, uint32_t _height );
	void finalize();

protected:

	bool initVulkan();
	bool initSwapchain();
	bool initCommands();
	bool initSyncStructures();

	const bool m_useValidationLayers = true;

	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice;
	VkDevice m_device;
	VkSurfaceKHR m_surface;

};


}