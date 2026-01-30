#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace wv {

class Swapchain
{
public:
	Swapchain( VkDevice _device, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface ) : 
		m_device{ _device },
		m_physicalDevice{ _physicalDevice },
		m_surface{ _surface }
	{ }

	void createSwapchain( uint32_t _width, uint32_t _height );
	void destroySwapchain();
	
	VkResult acquireNextImage( VkSemaphore _acquireSemaphore, uint32_t* _outIndex ) const;
	
	VkResult present( uint32_t _imageIndex, VkQueue _queue ) const;

	VkImage     getSwapchainImage( uint32_t _index )     const { return m_swapchainImages.at( _index ); }
	VkImageView getSwapchainImageView( uint32_t _index ) const { return m_swapchainImageViews.at( _index ); }
	VkSemaphore getSubmitSemaphore( uint32_t _index )    const { return m_submitSemaphores.at( _index ); }
	VkExtent2D  getExtent()                              const { return m_swapchainExtent; }

private:
	VkDevice         m_device = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR     m_surface = VK_NULL_HANDLE;

	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
	VkFormat       m_swapchainImageFormat = VK_FORMAT_UNDEFINED;

	std::vector<VkImage>     m_swapchainImages = {};
	std::vector<VkImageView> m_swapchainImageViews = {};
	std::vector<VkSemaphore> m_submitSemaphores = {};
	VkExtent2D m_swapchainExtent = {};

};

}