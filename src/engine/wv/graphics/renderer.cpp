#include "renderer.h"

#include <auxiliary/vk-bootstrap/VkBootstrap.h>

#include <wv/application.h>
#include <wv/display_driver.h>
#include <wv/debug/log.h>

#include <wv/entity/world.h>
#include <wv/graphics/viewport.h>

#ifdef WV_SUPPORT_SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <sdl/display_driver_sdl.h>
#endif

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////

VkImageSubresourceRange imageSubresourceRange( VkImageAspectFlags _aspectMask )
{
	VkImageSubresourceRange subImage{};
	subImage.aspectMask = _aspectMask;
	subImage.baseMipLevel = 0;
	subImage.levelCount = VK_REMAINING_MIP_LEVELS;
	subImage.baseArrayLayer = 0;
	subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	return subImage;
}

///////////////////////////////////////////////////////////////////////////////////////

void transitionImage( VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout )
{
	VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
	
	imageBarrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = currentLayout;
	imageBarrier.newLayout = newLayout;

	VkImageAspectFlags aspectMask = ( newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrier.subresourceRange = imageSubresourceRange( aspectMask );
	imageBarrier.image = image;

	VkDependencyInfo depInfo{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
	
	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2( cmd, &depInfo );
}

///////////////////////////////////////////////////////////////////////////////////////

VkSemaphoreSubmitInfo semaphoreSubmitInfo( VkPipelineStageFlags2 _stageMask, VkSemaphore _semaphore )
{
	VkSemaphoreSubmitInfo submitInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO };
	submitInfo.semaphore = _semaphore;
	submitInfo.stageMask = _stageMask;
	submitInfo.deviceIndex = 0;
	submitInfo.value = 1;

	return submitInfo;
}

///////////////////////////////////////////////////////////////////////////////////////

VkSubmitInfo2 submitInfo2( VkCommandBufferSubmitInfo* _cmd, VkSemaphoreSubmitInfo* _signalSemaphoreInfo, VkSemaphoreSubmitInfo* _waitSemaphoreInfo )
{
	VkSubmitInfo2 info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
	info.waitSemaphoreInfoCount = _waitSemaphoreInfo == nullptr ? 0 : 1;
	info.pWaitSemaphoreInfos    = _waitSemaphoreInfo;

	info.signalSemaphoreInfoCount = _signalSemaphoreInfo == nullptr ? 0 : 1;
	info.pSignalSemaphoreInfos    = _signalSemaphoreInfo;

	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos = _cmd;

	return info;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initialize()
{
	if ( !initVulkan() ) 
		return false;

	DisplayDriver* displayDriver = wv::Application::getSingleton()->getDisplayDriver();
	Vector2i windowSize = displayDriver->getWindowSize();

	if ( !initSwapchain( windowSize.x, windowSize.y ) ) 
		return false;

	if ( !initCommands() ) 
		return false;

	if ( !initSyncStructures() ) 
		return false;

	m_initialized = true;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Renderer::shutdown()
{
	if ( m_initialized )
	{
		vkDeviceWaitIdle( m_device );

		for ( uint32_t i = 0; i < FRAME_OVERLAP; i++ )
		{
			vkDestroyCommandPool( m_device, m_frames[ i ].commandPool, nullptr );

			vkDestroyFence( m_device, m_frames[ i ].fence, nullptr );
			vkDestroySemaphore( m_device, m_frames[ i ].acquireSemaphore, nullptr );
		}

		m_mainDeleteQueue.flush();

	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Renderer::prepare( uint32_t _width, uint32_t _height )
{
	

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Renderer::render( World* _world )
{
	vkWaitForFences( m_device, 1, &getCurrentFrame().fence, true, 1000000000 );
	vkResetFences( m_device, 1, &getCurrentFrame().fence );

	uint32_t swapchainImageIndex;
	vkAcquireNextImageKHR( m_device, m_swapchain, 1000000000, getCurrentFrame().acquireSemaphore, nullptr, &swapchainImageIndex );

	// Draw
	VkCommandBuffer cmd = getCurrentFrame().mainCommandBuffer;
	
	{
		// Command buffer reset & begin
		vkResetCommandBuffer( cmd, 0 );

		VkCommandBufferBeginInfo cmdBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer( cmd, &cmdBeginInfo );

		// Clear image
		transitionImage( cmd, m_swapchainImages[ swapchainImageIndex ], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL );

		float flash = std::abs( std::sin( m_frameNumber / 120.f ) );
		VkClearColorValue clearValue = { { 0.0f, 0.0f, flash, 1.0f } };
		VkImageSubresourceRange clearRange = imageSubresourceRange( VK_IMAGE_ASPECT_COLOR_BIT );

		vkCmdClearColorImage( cmd, m_swapchainImages[ swapchainImageIndex ], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange );

		transitionImage( cmd, m_swapchainImages[ swapchainImageIndex ], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );

		vkEndCommandBuffer( cmd );
	}

	// Submit

	VkCommandBufferSubmitInfo cmdInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
	cmdInfo.commandBuffer = cmd;

	VkSemaphoreSubmitInfo waitInfo   = semaphoreSubmitInfo( VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, getCurrentFrame().acquireSemaphore );
	VkSemaphoreSubmitInfo signalInfo = semaphoreSubmitInfo( VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, m_submitSemaphores[ swapchainImageIndex ] );

	VkSubmitInfo2 submitInfo = submitInfo2( &cmdInfo, &signalInfo, &waitInfo );
	vkQueueSubmit2( m_graphicsQueue, 1, &submitInfo, getCurrentFrame().fence );

	// Present

	VkPresentInfoKHR presentInfo{ .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.pSwapchains = &m_swapchain;
	presentInfo.swapchainCount = 1;
	presentInfo.pWaitSemaphores = &m_submitSemaphores[ swapchainImageIndex ];
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;
	
	vkQueuePresentKHR( m_graphicsQueue, &presentInfo );

	m_frameNumber++;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Renderer::finalize()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initVulkan()
{
	vkb::InstanceBuilder builder;
	auto ret = builder.set_app_name( "Wyvern" )
		.request_validation_layers( m_useValidationLayers )
		.use_default_debug_messenger()
		.require_api_version( 1, 3, 0 )
		.build();

	if ( !ret.has_value() )
	{
		WV_LOG_ERROR( "Failed to create VkInstance\n" );
		return false;
	}

	vkb::Instance vkbInstance = ret.value();
	m_instance = vkbInstance.instance;
	m_debugMessenger = vkbInstance.debug_messenger;

	m_mainDeleteQueue.push( [ & ]() {
		vkb::destroy_debug_utils_messenger( m_instance, m_debugMessenger );
		vkDestroyInstance( m_instance, nullptr );
	} );

	DisplayDriver* displayDriver = wv::Application::getSingleton()->getDisplayDriver();
	
#ifdef WV_SUPPORT_SDL2
	{
		DisplayDriverSDL* sdlDisplayDriver = static_cast<DisplayDriverSDL*>( displayDriver );
		SDL_Vulkan_CreateSurface( sdlDisplayDriver->getSDLWindowContext(), m_instance, &m_surface);
	}
#endif

	VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;

	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing  = true;
	
	vkb::PhysicalDeviceSelector selector{ vkbInstance };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version( 1, 3 )
		.set_required_features_13( features )
		.set_required_features_12( features12 )
		.set_surface( m_surface )
		.select()
		.value();

	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkb::Device vkbDevice = deviceBuilder.build().value();
	
	m_device = vkbDevice.device;
	m_physicalDevice = physicalDevice.physical_device;

	m_mainDeleteQueue.push( [ & ]() {
		vkDestroySurfaceKHR( m_instance, m_surface, nullptr );
		vkDestroyDevice( m_device, nullptr );
	} );

	m_graphicsQueue = vkbDevice.get_queue( vkb::QueueType::graphics ).value();
	m_graphicsQueueFamily = vkbDevice.get_queue_index( vkb::QueueType::graphics ).value();

	VmaAllocatorCreateInfo vmaInfo{};
	vmaInfo.physicalDevice = m_physicalDevice;
	vmaInfo.device = m_device;
	vmaInfo.instance = m_instance;
	vmaInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator( &vmaInfo, &m_allocator );

	m_mainDeleteQueue.push( [ & ]() {
		vmaDestroyAllocator( m_allocator );
	} );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initSwapchain( uint32_t _width, uint32_t _height )
{
	createSwapchain( _width, _height );

	m_mainDeleteQueue.push( [ & ]() {
		destroySwapchain();
	} );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initCommands()
{
	VkCommandPoolCreateInfo commandPoolInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolInfo.queueFamilyIndex = m_graphicsQueueFamily;

	for ( uint32_t i = 0; i < FRAME_OVERLAP; i++ )
	{
		vkCreateCommandPool( m_device, &commandPoolInfo, nullptr, &m_frames[ i ].commandPool );

		VkCommandBufferAllocateInfo cmdAllocInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		cmdAllocInfo.commandPool = m_frames[ i ].commandPool;
		cmdAllocInfo.commandBufferCount = 1;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		vkAllocateCommandBuffers( m_device, &cmdAllocInfo, &m_frames[ i ].mainCommandBuffer );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initSyncStructures()
{
	VkFenceCreateInfo fenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VkSemaphoreCreateInfo semaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

	for ( uint32_t i = 0; i < FRAME_OVERLAP; i++ )
	{
		vkCreateFence( m_device, &fenceCreateInfo, nullptr, &m_frames[ i ].fence );

		vkCreateSemaphore( m_device, &semaphoreCreateInfo, nullptr, &m_frames[ i ].acquireSemaphore );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Renderer::createSwapchain( uint32_t _width, uint32_t _height )
{
	vkb::SwapchainBuilder builder{ m_physicalDevice, m_device, m_surface };
	
	m_swapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

	vkb::Swapchain vkbSwapchain = builder
		.set_desired_format( VkSurfaceFormatKHR{ .format = m_swapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR } )
		.set_desired_present_mode( VK_PRESENT_MODE_FIFO_KHR )
		.set_desired_extent( _width, _height )
		.add_image_usage_flags( VK_IMAGE_USAGE_TRANSFER_DST_BIT )
		.build()
		.value();

	m_swapchainExtent = vkbSwapchain.extent;
	m_swapchain = vkbSwapchain.swapchain;
	m_swapchainImages = vkbSwapchain.get_images().value();
	m_swapchainImageViews = vkbSwapchain.get_image_views().value();

	VkSemaphoreCreateInfo semaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	m_submitSemaphores.resize( m_swapchainImages.size() );
	for ( size_t i = 0; i < m_swapchainImages.size(); i++ )
		vkCreateSemaphore( m_device, &semaphoreCreateInfo, nullptr, &m_submitSemaphores[ i ] );

}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Renderer::destroySwapchain()
{
	vkDestroySwapchainKHR( m_device, m_swapchain, nullptr );
	
	for ( auto imageView : m_swapchainImageViews )
		vkDestroyImageView( m_device, imageView, nullptr );

	for ( auto semaphore : m_submitSemaphores )
		vkDestroySemaphore( m_device, semaphore, nullptr );

	m_swapchainImages.clear();
	m_swapchainImageViews.clear();
	m_submitSemaphores.clear();
}
