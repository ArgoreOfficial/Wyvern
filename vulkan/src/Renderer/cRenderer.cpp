#include "cRenderer.h"

#include "../Window/cWindow.h"

#include <vulkan/vulkan_win32.h>

#include <stdio.h>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

/* TODO: remove this? */
#define VECTOR_VK_GET( _vec, _func, ... )            \
uint32_t num_##_func;                                \
_func( __VA_ARGS__, &num_##_func, nullptr );         \
if ( num_##_func != 0 )                              \
{                                                    \
	_vec.resize( num_##_func );                      \
	_func( __VA_ARGS__, &num_##_func, _vec.data() ); \
}                                                    \


cm::cRenderer::cRenderer()
{
}

cm::cRenderer::~cRenderer( void )
{
}

void cm::cRenderer::init( cWindow* _window )
{
	m_active_window = _window;

	createInstance();
	createSurface();
	selectPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
}

void cm::cRenderer::destroy( void )
{
	vkDestroySwapchainKHR( m_device, m_swapchain, nullptr );
	vkDestroyDevice( m_device, nullptr );
	vkDestroySurfaceKHR( m_instance, m_surface, nullptr );
	vkDestroyInstance( m_instance, nullptr );
}

void cm::cRenderer::createInstance( void )
{
	VkApplicationInfo app_info{};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Vulkan Backend";
	app_info.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
	app_info.pEngineName = "none";
	app_info.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
	app_info.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = m_active_window->getRequiredInstanceExtensions( &glfwExtensionCount );

	create_info.enabledExtensionCount = glfwExtensionCount;
	create_info.ppEnabledExtensionNames = glfwExtensions;
	create_info.enabledLayerCount = 0;


	VkResult result = vkCreateInstance( &create_info, nullptr, &m_instance );
	if( result == VK_SUCCESS )
		printf( "[INFO] Created Vulkan Instance.\n" );
	else
		printErrorResult( "[FATAL] Failed to create Vulkan Instance:", result);
}

void cm::cRenderer::selectPhysicalDevice( void )
{
	uint32_t num_devices = 0;
	vkEnumeratePhysicalDevices( m_instance, &num_devices, nullptr );

	if ( num_devices == 0 )
	{
		printf( "[FATAL] Found no devices with Vulkan support!\n" );
		return;
	}

	std::vector<VkPhysicalDevice> devices( num_devices );
	vkEnumeratePhysicalDevices( m_instance, &num_devices, devices.data() );

	int selected;
	for ( int i = 0; i < num_devices; i++ )
	{
		if ( isDeviceSuitable( devices[ i ] ) )
		{
			m_physical_device = devices[ i ];
			selected = i;
			break;
		}
	}

	if ( !m_physical_device )
		printf( "[FATAL] Failed to select device!\n" );
	else
		printf( "[Info] Selected physical device %i.\n", selected );
}

void cm::cRenderer::createLogicalDevice( void )
{
	sQueueFamilyIndices indices = findQueueFamilies( m_physical_device );
	float queue_priority = 1.0f;
	
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };

	for ( uint32_t queue_family : unique_queue_families )
	{
		VkDeviceQueueCreateInfo queue_create_info{};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = queue_family;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_priority;

		queue_create_infos.push_back( queue_create_info );
	}

	/* create logical device */
	VkPhysicalDeviceFeatures device_features{};
	VkDeviceCreateInfo create_info{};
	create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount    = (uint32_t)queue_create_infos.size();
	create_info.pQueueCreateInfos       = queue_create_infos.data();
	create_info.pEnabledFeatures        = &device_features;
	create_info.enabledExtensionCount   = (uint32_t)( m_device_extensions.size() );
	create_info.ppEnabledExtensionNames = m_device_extensions.data();
	create_info.enabledLayerCount       = 0;

	VkResult result = vkCreateDevice( m_physical_device, &create_info, nullptr, &m_device );
	if ( result == VK_SUCCESS )
		printf( "[INFO] Created Logical Device.\n" );
	else
		printErrorResult( "[FATAL] Failed to create Logical Device:", result );
	
	vkGetDeviceQueue( m_device, indices.graphics_family.value(), 0, &m_graphics_queue );
	vkGetDeviceQueue( m_device, indices.present_family.value(),  0, &m_present_queue );


}

void cm::cRenderer::createSurface( void )
{
	m_active_window->createSurface( m_instance, &m_surface );
}

void cm::cRenderer::createSwapChain( void )
{
	sSwapChainSupportDetails sc_support = querySwapChainSupport( m_physical_device );

	VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat( sc_support.formats );
	VkPresentModeKHR present_mode     = chooseSwapPresentMode  ( sc_support.present_modes );
	VkExtent2D extent                 = chooseSwapExtent       ( sc_support.capabilities );

	uint32_t num_images = sc_support.capabilities.minImageCount + 1;
	if ( sc_support.capabilities.maxImageCount > 0 && num_images > sc_support.capabilities.maxImageCount )
		num_images = sc_support.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR create_info{};
	create_info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface          = m_surface;
	create_info.minImageCount    = num_images;
	create_info.imageFormat      = surface_format.format;
	create_info.imageColorSpace  = surface_format.colorSpace;
	create_info.imageExtent      = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	sQueueFamilyIndices indices = findQueueFamilies( m_physical_device );
	uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };

	if ( indices.graphics_family != indices.present_family )
	{
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indices;
	}
	else
	{
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
	}

	create_info.preTransform = sc_support.capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE; // TODO: allow for recreation

	VkResult result = vkCreateSwapchainKHR( m_device, &create_info, nullptr, &m_swapchain );

	if ( result == VK_SUCCESS )
		printf("[INFO] Created Swapchain.");
	else
		printErrorResult( "[FATAL] Failed to create Swapchain:", result);

	/* get swapchain images */
	VECTOR_VK_GET( m_swapchain_images, vkGetSwapchainImagesKHR, m_device, m_swapchain );

	m_swapchain_image_format = surface_format.format;
	m_swapchain_extent       = extent;
}

void cm::cRenderer::printErrorResult( const std::string& _message, VkResult _result )
{
	printf( "%s\n   ", _message.c_str() );

	switch ( _result )
	{
	case VK_ERROR_OUT_OF_HOST_MEMORY:        printf( "OUT_OF_HOST_MEMORY" );        break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:      printf( "OUT_OF_DEVICE_MEMORY" );      break;
	case VK_ERROR_INITIALIZATION_FAILED:     printf( "INITIALIZATION_FAILED" );     break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:     printf( "EXTENSION_NOT_PRESENT" );     break;
	case VK_ERROR_FEATURE_NOT_PRESENT:       printf( "FEATURE_NOT_PRESENT" );       break;
	case VK_ERROR_TOO_MANY_OBJECTS:          printf( "TOO_MANY_OBJECTS" );          break;
	case VK_ERROR_DEVICE_LOST:               printf( "DEVICE_LOST" );               break;
	case VK_ERROR_LAYER_NOT_PRESENT:         printf( "LAYER_NOT_PRESENT" );         break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:       printf( "INCOMPATIBLE_DRIVER" );       break;
	case VK_ERROR_SURFACE_LOST_KHR:          printf( "SURFACE_LOST_KHR" );          break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:  printf( "NATIVE_WINDOW_IN_USE_KHR" );  break;
	case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: printf( "COMPRESSION_EXHAUSTED_EXT" ); break;
	}

	printf( "\n" );
}

bool cm::cRenderer::isDeviceSuitable( VkPhysicalDevice _device )
{
	sQueueFamilyIndices indices = findQueueFamilies( _device );
	bool extensions_supported = checkDeviceExtensionSupport( _device );

	/* swap chain support */
	bool swap_chain_adequate = false;
	if ( extensions_supported )
	{
		sSwapChainSupportDetails swap_chain_support = querySwapChainSupport( _device );
		swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
	}

	return indices.isComplete() && extensions_supported && swap_chain_adequate;
}

bool cm::cRenderer::checkDeviceExtensionSupport( VkPhysicalDevice _device )
{
	uint32_t num_extensions;
	vkEnumerateDeviceExtensionProperties( _device, nullptr, &num_extensions, nullptr );

	std::vector<VkExtensionProperties> available_extensions( num_extensions );
	vkEnumerateDeviceExtensionProperties( _device, nullptr, &num_extensions, available_extensions.data() );

	std::set<std::string> required_extensions( m_device_extensions.begin(), m_device_extensions.end() );

	for ( const auto& extension : available_extensions )
		required_extensions.erase( extension.extensionName );
	
	return required_extensions.empty();
}

sQueueFamilyIndices cm::cRenderer::findQueueFamilies( VkPhysicalDevice _device )
{
	sQueueFamilyIndices indices;
	uint32_t num_families;
	vkGetPhysicalDeviceQueueFamilyProperties( _device, &num_families, nullptr );

	std::vector<VkQueueFamilyProperties> families( num_families );
	vkGetPhysicalDeviceQueueFamilyProperties( _device, &num_families, families.data() );

	for ( int i = 0; i < num_families; i++ )
	{
		if ( families[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT )
			indices.graphics_family = i;

		/* check if the device has presentation support */
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR( _device, i, m_surface, &present_support );
		if ( present_support )
			indices.present_family = i;
		
		/* if complete, break */
		if ( indices.isComplete() )
			break;
	}

	return indices;
}

sSwapChainSupportDetails cm::cRenderer::querySwapChainSupport( VkPhysicalDevice _device )
{
	sSwapChainSupportDetails details;

	/* capabilities */
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( _device, m_surface, &details.capabilities );

	/* format */
	VECTOR_VK_GET( details.formats, vkGetPhysicalDeviceSurfaceFormatsKHR, _device, m_surface );

	/* present modes */
	VECTOR_VK_GET( details.present_modes, vkGetPhysicalDeviceSurfacePresentModesKHR, _device, m_surface );

	return details;
}

VkSurfaceFormatKHR cm::cRenderer::chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& _available_formats )
{
	for ( const auto& available_format : _available_formats )
		if ( available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
			return available_format;

	return _available_formats[ 0 ];
}

VkPresentModeKHR cm::cRenderer::chooseSwapPresentMode( const std::vector<VkPresentModeKHR>& _available_present_modes )
{
	for ( const auto& available_present_mode : _available_present_modes )
		if ( available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
			return available_present_mode;
	
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D cm::cRenderer::chooseSwapExtent( const VkSurfaceCapabilitiesKHR& _capabilities )
{
	if ( _capabilities.currentExtent.width != UINT32_MAX )
		return _capabilities.currentExtent;
	else
	{
		int width, height;
		m_active_window->getFramebufferSize( &width, &height );

		VkExtent2D extent = {
			(uint32_t)width,
			(uint32_t)height
		};

		extent.width  = std::clamp( extent.width,  _capabilities.minImageExtent.width,  _capabilities.maxImageExtent.width  );
		extent.height = std::clamp( extent.height, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height );

		return extent;
	}
}

