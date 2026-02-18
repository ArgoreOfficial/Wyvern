#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace wv {

struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class Swapchain
{
public:
	Swapchain( VkDevice _device, VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface ) : 
		m_device{ _device },
		m_physicalDevice{ _physicalDevice },
		m_surface{ _surface }
	{ }

	void createSwapchain( uint32_t _width, uint32_t _height, VkSwapchainKHR _oldSwapchain = VK_NULL_HANDLE );
	void destroySwapchain();
	
	void recreateSwapchain( uint32_t _width, uint32_t _height );

	VkResult acquireNextImage( VkSemaphore _acquireSemaphore, uint32_t* _outIndex ) const;
	
	VkResult present( uint32_t _imageIndex, VkQueue _queue ) const;

	VkImage     getSwapchainImage( uint32_t _index )     const { return m_swapchainImages.at( _index ); }
	VkImageView getSwapchainImageView( uint32_t _index ) const { return m_swapchainImageViews.at( _index ); }
	VkSemaphore getSubmitSemaphore( uint32_t _index )    const { return m_submitSemaphores.at( _index ); }
	VkExtent2D  getExtent()                              const { return m_swapchainExtent; }
	VkFormat    getFormat()                              const { return m_format.format; }

private:
	SwapchainSupportDetails querySwapchainSupport() const;

	VkSurfaceFormatKHR chooseSurfaceFormat( VkFormat _desiredFormat, VkColorSpaceKHR _desiredColorSpace ) const;
	VkPresentModeKHR   chooseSwapPresentMode( VkPresentModeKHR _desiredPresentMode ) const;
	VkExtent2D         chooseSwapExtent( uint32_t _desiredWidth, uint32_t _desiredHeight ) const;

	// gets images directly from swapchain
	std::vector<VkImage> getSwapchainImages() const;
	std::vector<VkImageView> createSwapchainImageViews( const std::vector<VkImage>& _images ) const;

	SwapchainSupportDetails m_supportDetails;
	VkDevice         m_device = VK_NULL_HANDLE;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkSurfaceKHR     m_surface = VK_NULL_HANDLE;

	VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
	
	VkSurfaceFormatKHR m_format{};

	std::vector<VkImage>     m_swapchainImages = {};
	std::vector<VkImageView> m_swapchainImageViews = {};
	std::vector<VkSemaphore> m_submitSemaphores = {};
	VkExtent2D m_swapchainExtent = {};

};

}