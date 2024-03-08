#pragma once

#include "Framework/CoreTypes.h"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

///////////////////////////////////////////////////////////////////////////////////////

struct GLFWwindow;

///////////////////////////////////////////////////////////////////////////////////////

namespace cm
{

///////////////////////////////////////////////////////////////////////////////////////

	class cWindow;

///////////////////////////////////////////////////////////////////////////////////////

	class cRenderer
	{
	public:

		 cRenderer( void );
		~cRenderer( void );

		void init   ( cWindow* _window );
		void destroy( void );
		void draw   ( void );


///////////////////////////////////////////////////////////////////////////////////////

	private:

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT _severity, VkDebugUtilsMessageTypeFlagsEXT _type, const VkDebugUtilsMessengerCallbackDataEXT* _data, void* _user_data );

		void recreateSwapchain();
		void createInstance        ( void );
		void setupDebugMessenger   ( void );
		void selectPhysicalDevice  ( void );
		void createLogicalDevice   ( void );
		void createSurface         ( void );
		void createSwapChain       ( void );
		void createImageViews      ( void );
		void createRenderPass      ( void );
		void createGraphicsPipeline( void );
		void createFramebuffers    ( void );
		void createCommandPool     ( void );
		void createCommandBuffers  ( void );
		void createSyncObjects     ( void );

		void cleanupSwapchain();

		void recordCommandBuffer( VkCommandBuffer _command_buffer, uint32_t _image_index );
		void printErrorResult   ( const std::string& _message, VkResult _result );

		void onResize( uint32_t _width, uint32_t _height );

		bool                     checkValidationLayerSupport( void );
		bool                     isDeviceSuitable           ( VkPhysicalDevice _device );
		bool                     checkDeviceExtensionSupport( VkPhysicalDevice _device );
		sSwapChainSupportDetails querySwapChainSupport      ( VkPhysicalDevice _device );
		sQueueFamilyIndices      findQueueFamilies          ( VkPhysicalDevice _device );

		std::vector<const char*> getRequiredExtensions();

		VkSurfaceFormatKHR chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& _available_formats );
		VkPresentModeKHR   chooseSwapPresentMode  ( const std::vector<VkPresentModeKHR>& _available_present_modes );
		VkExtent2D         chooseSwapExtent       ( const VkSurfaceCapabilitiesKHR& _capabilities );

		VkShaderModule createShaderModule( const std::vector<char>& _code );

///////////////////////////////////////////////////////////////////////////////////////

		const std::vector<const char*> m_device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		const std::vector<const char*> m_validation_layers = {
			"VK_LAYER_KHRONOS_validation"
		};

	#if defined( _DEBUG )
		const bool m_enable_validation_layers = true;
	#else
		const bool m_enable_validation_layers = false;
	#endif

		const int m_max_frames_in_flight = 2;

///////////////////////////////////////////////////////////////////////////////////////

		cWindow* m_active_window;

		/* debug */
		VkDebugUtilsMessengerEXT m_debug_messenger;

		/* device */
		VkInstance       m_instance        = VK_NULL_HANDLE;
		VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
		VkDevice         m_device          = VK_NULL_HANDLE;
		VkQueue          m_graphics_queue  = VK_NULL_HANDLE;
		VkQueue          m_present_queue   = VK_NULL_HANDLE;

		/* window */
		VkSurfaceKHR   m_surface   = VK_NULL_HANDLE;
		VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
		
		std::vector<VkImage>       m_swapchain_images;
		std::vector<VkImageView>   m_swapchain_image_views;
		std::vector<VkFramebuffer> m_swapchain_framebuffers;
		
		VkFormat   m_swapchain_image_format;
		VkExtent2D m_swapchain_extent;

		uint32_t m_current_frame = 0;
		bool m_framebuffer_resized = false;

		/* pipeline */
		VkRenderPass     m_render_pass       = VK_NULL_HANDLE;
		VkPipelineLayout m_pipeline_layout   = VK_NULL_HANDLE;
		VkPipeline       m_graphics_pipeline = VK_NULL_HANDLE;

		/* commands */
		VkCommandPool m_command_pool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_command_buffers;

		/* synchronization */
		std::vector<VkSemaphore> m_image_available_semaphores;
		std::vector<VkSemaphore> m_render_finished_semaphores;
		std::vector<VkFence>     m_in_flight_fences;

///////////////////////////////////////////////////////////////////////////////////////

	};

}