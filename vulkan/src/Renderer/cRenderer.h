#pragma once

#include "Framework/CoreTypes.h"

#include <vulkan/vulkan.h>

#include <string>

struct GLFWwindow;

namespace cm
{
	class cWindow;

	class cRenderer
	{
	public:
		 cRenderer( void );
		~cRenderer( void );

		void init   ( cWindow* _window );
		void destroy( void );

	private:

		void createInstance        ( void );
		void selectPhysicalDevice  ( void );
		void createLogicalDevice   ( void );
		void createSurface         ( void );
		void createSwapChain       ( void );
		void createImageViews      ( void );
		void createGraphicsPipeline( void );

		void printErrorResult( const std::string& _message, VkResult _result );

		bool                isDeviceSuitable           ( VkPhysicalDevice _device );
		bool                checkDeviceExtensionSupport( VkPhysicalDevice _device );
		sQueueFamilyIndices findQueueFamilies          ( VkPhysicalDevice _device );
		sSwapChainSupportDetails querySwapChainSupport ( VkPhysicalDevice _device );

		VkSurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& _available_formats );
		VkPresentModeKHR   chooseSwapPresentMode  ( const std::vector<VkPresentModeKHR>& _available_present_modes );
		VkExtent2D         chooseSwapExtent       ( const VkSurfaceCapabilitiesKHR& _capabilities );

		VkShaderModule createShaderModule( const std::vector<char>& _code );

		const std::vector<const char*> m_device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		cWindow* m_active_window;

		/* device */
		VkInstance       m_instance        = VK_NULL_HANDLE;
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkDevice         m_device          = VK_NULL_HANDLE;
		VkQueue          m_graphics_queue  = VK_NULL_HANDLE;
		VkQueue          m_present_queue   = VK_NULL_HANDLE;

		/* window */
		VkSurfaceKHR   m_surface   = VK_NULL_HANDLE;
		VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
		
		std::vector<VkImage>     m_swapchain_images;
		std::vector<VkImageView> m_swapchain_image_views;
		
		VkFormat   m_swapchain_image_format;
		VkExtent2D m_swapchain_extent;

	};
}