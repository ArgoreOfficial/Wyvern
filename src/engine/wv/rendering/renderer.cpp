#include "renderer.h"

#include <auxiliary/vk-bootstrap/VkBootstrap.h>

#include <wv/application.h>
#include <wv/debug/log.h>
#include <wv/display_driver.h>
#include <wv/entity/world.h>
#include <wv/filesystem/file_system.h>
#include <wv/rendering/viewport.h>

#ifdef WV_SUPPORT_SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <sdl/display_driver_sdl.h>
#endif

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <stdio.h>

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

VkImageCreateInfo imageCreateInfo( VkFormat _format, VkImageUsageFlags _usageFlags, VkExtent3D _extent )
{
	VkImageCreateInfo info{ .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	info.imageType = VK_IMAGE_TYPE_2D;

	info.format = _format;
	info.extent = _extent;

	info.mipLevels = 1;
	info.arrayLayers = 1;

	info.samples = VK_SAMPLE_COUNT_1_BIT;

	info.tiling = VK_IMAGE_TILING_OPTIMAL;
	info.usage = _usageFlags;

	return info;
}

///////////////////////////////////////////////////////////////////////////////////////

VkImageViewCreateInfo imageViewCreateInfo( VkFormat _format, VkImage _image, VkImageAspectFlags _aspectFlags )
{
	VkImageViewCreateInfo info{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.image = _image;
	info.format = _format;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;
	info.subresourceRange.aspectMask = _aspectFlags;

	return info;
}

///////////////////////////////////////////////////////////////////////////////////////

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

	if ( !initDescriptors() )
		return false;



	// create compute pipeline layout
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutInfo.pSetLayouts = &m_drawImageDescriptorLayout;
		pipelineLayoutInfo.setLayoutCount = 1;
		vkCreatePipelineLayout( m_device, &pipelineLayoutInfo, nullptr, &m_gradientPipelineLayout );
	}


	// create shader module
	{
		IFileSystem* fileSystem = wv::Application::getSingleton()->getFileSystem();

		std::vector<uint8_t> buffer = fileSystem->loadEntireFile( "shaders/gradient.comp.spv" );
		if ( buffer.empty() )
			return false;

		VkShaderModule shaderModule = createShaderModule( (uint32_t*)buffer.data(), buffer.size() );
		if ( shaderModule == VK_NULL_HANDLE )
			return false;

		m_gradientPipeline = createComputePipeline( shaderModule, m_gradientPipelineLayout, "main" );
		if ( m_gradientPipeline == VK_NULL_HANDLE )
			return false;

		// destroy module, it's not needed anymore
		vkDestroyShaderModule( m_device, shaderModule, nullptr );

		m_mainDeleteQueue.push( [ & ]() {
			vkDestroyPipelineLayout( m_device, m_gradientPipelineLayout, nullptr );
			vkDestroyPipeline( m_device, m_gradientPipeline, nullptr );
		} );
	}

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
			WV_FREE( m_frames[ i ].mainCommandBuffer );

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
	CommandBuffer* cmd = getCurrentFrame().mainCommandBuffer;
	cmd->reset();


	{
		m_drawExtent.width = m_drawImage.imageExtent.width;
		m_drawExtent.height = m_drawImage.imageExtent.height;
		cmd->begin();

		cmd->transitionImage( m_drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL );

		// draw background colour
		drawBackground( cmd );

		cmd->transitionImage( m_drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );
		cmd->transitionImage( m_swapchainImages[ swapchainImageIndex ], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

		// copy draw to swapchain
		cmd->copyImageToImage( m_drawImage.image, m_swapchainImages[ swapchainImageIndex ], m_drawExtent, m_swapchainExtent );

		cmd->transitionImage( m_swapchainImages[ swapchainImageIndex ], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );

		cmd->end();
	}

	// Submit

	VkSemaphoreSubmitInfo waitInfo = semaphoreSubmitInfo( VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, getCurrentFrame().acquireSemaphore );
	VkSemaphoreSubmitInfo signalInfo = semaphoreSubmitInfo( VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, m_submitSemaphores[ swapchainImageIndex ] );
	cmd->submit( m_graphicsQueue, &waitInfo, &signalInfo, getCurrentFrame().fence );

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
		SDL_Vulkan_CreateSurface( sdlDisplayDriver->getSDLWindowContext(), m_instance, &m_surface );
	}
#endif

	VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;

	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing = true;

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

	VkExtent3D drawImageExtent = { _width, _height, 1 };
	m_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	m_drawImage.imageExtent = drawImageExtent;

	VkImageUsageFlags drawImageUsage{};
	drawImageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	drawImageUsage |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkImageCreateInfo drawImageInfo = imageCreateInfo( m_drawImage.imageFormat, drawImageUsage, drawImageExtent );
	VmaAllocationCreateInfo drawImageAllocInfo{};
	drawImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY; // needed?
	drawImageAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	vmaCreateImage( m_allocator, &drawImageInfo, &drawImageAllocInfo, &m_drawImage.image, &m_drawImage.allocation, nullptr );

	VkImageViewCreateInfo drawImageViewInfo = imageViewCreateInfo( m_drawImage.imageFormat, m_drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT );
	vkCreateImageView( m_device, &drawImageViewInfo, nullptr, &m_drawImage.imageView );

	m_mainDeleteQueue.push( [ & ]() {
		vkDestroyImageView( m_device, m_drawImage.imageView, nullptr );
		vmaDestroyImage( m_allocator, m_drawImage.image, m_drawImage.allocation );
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

		m_frames[ i ].mainCommandBuffer = WV_NEW( CommandBuffer, m_device, m_frames[ i ].commandPool );
	}

	vkCreateCommandPool( m_device, &commandPoolInfo, nullptr, &m_immediateCommandPool );
	m_immediateCommandBuffer = WV_NEW( CommandBuffer, m_device, m_immediateCommandPool );
	m_mainDeleteQueue.push( [ = ]() {
		WV_FREE( m_immediateCommandBuffer );

		vkDestroyCommandPool( m_device, m_immediateCommandPool, nullptr );
	} );

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

	vkCreateFence( m_device, &fenceCreateInfo, nullptr, &m_immediateFence );
	m_mainDeleteQueue.push( [ = ]() { vkDestroyFence( m_device, m_immediateFence, nullptr ); } );

	return true;
}

bool wv::Renderer::initDescriptors()
{
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes =
	{
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 }
	};

	m_globalDescriptorAllocator.initialize( m_device, 10, sizes );


	// create descriptor set layout
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		{
			VkDescriptorSetLayoutBinding newbind{
				.binding = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
				.descriptorCount = 1
			};
			bindings.push_back( newbind );
		}

		for ( auto& b : bindings )
			b.stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;

		VkDescriptorSetLayoutCreateInfo info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		info.pNext = nullptr;

		info.pBindings = bindings.data();
		info.bindingCount = (uint32_t)bindings.size();
		//info.flags = flags;

		VkDescriptorSetLayout set;
		vkCreateDescriptorSetLayout( m_device, &info, nullptr, &set );

		m_drawImageDescriptorLayout = set;
	}

	// create and update descriptor set layout
	{
		m_drawImageDescriptors = m_globalDescriptorAllocator.allocate( m_device, m_drawImageDescriptorLayout );

		VkDescriptorImageInfo imgInfo{};
		imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imgInfo.imageView = m_drawImage.imageView;

		VkWriteDescriptorSet drawImageWrite{ .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		drawImageWrite.dstBinding = 0;
		drawImageWrite.dstSet = m_drawImageDescriptors;
		drawImageWrite.descriptorCount = 1;
		drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		drawImageWrite.pImageInfo = &imgInfo;

		vkUpdateDescriptorSets( m_device, 1, &drawImageWrite, 0, nullptr );
	}

	m_mainDeleteQueue.push( [ & ]() {
		m_globalDescriptorAllocator.destroy( m_device );

		vkDestroyDescriptorSetLayout( m_device, m_drawImageDescriptorLayout, nullptr );
	} );

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

void wv::Renderer::drawBackground( CommandBuffer* _cmd )
{
//	float flash = std::abs( std::sin( m_frameNumber / 120.f ) );
//	VkClearColorValue clearValue = { { 0.0f, 0.0f, flash, 1.0f } };
//
//	_cmd->clearColorImage( m_drawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue );

	_cmd->bindPipeline( VK_PIPELINE_BIND_POINT_COMPUTE, m_gradientPipeline );
	
	_cmd->bindDescriptorSets( VK_PIPELINE_BIND_POINT_COMPUTE, m_gradientPipelineLayout, 0, 1, &m_drawImageDescriptors );

	_cmd->dispatch( std::ceil( m_drawExtent.width / 16.0 ), std::ceil( m_drawExtent.height / 16.0 ), 1 );
}

VkShaderModule wv::Renderer::createShaderModule( uint32_t* _data, size_t _dataSize )
{
	VkShaderModule shaderModule{ VK_NULL_HANDLE };

	VkShaderModuleCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	createInfo.codeSize = _dataSize;
	createInfo.pCode = _data;

	if ( vkCreateShaderModule( m_device, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
		return VK_NULL_HANDLE;

	return shaderModule;
}

VkPipeline wv::Renderer::createComputePipeline( VkShaderModule _shaderModule, VkPipelineLayout _layout, const char* _entryPoint )
{
	VkPipelineShaderStageCreateInfo stageInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	stageInfo.module = _shaderModule;
	stageInfo.pName = _entryPoint;

	VkComputePipelineCreateInfo compPipelinCreateInfo{ .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO };
	compPipelinCreateInfo.layout = _layout;
	compPipelinCreateInfo.stage = stageInfo;

	VkPipeline pipeline{ VK_NULL_HANDLE };
	if ( vkCreateComputePipelines( m_device, VK_NULL_HANDLE, 1, &compPipelinCreateInfo, nullptr, &pipeline ) != VK_SUCCESS )
		return VK_NULL_HANDLE;

	return pipeline;
}

void wv::Renderer::immediateCmdSubmit( std::function<void( CommandBuffer& _cmd )>&& _func )
{
	vkResetFences( m_device, 1, &m_immediateFence );
	m_immediateCommandBuffer->reset();
	m_immediateCommandBuffer->begin();

	_func( *m_immediateCommandBuffer );

	m_immediateCommandBuffer->end();
	m_immediateCommandBuffer->submit( m_graphicsQueue, nullptr, nullptr, m_immediateFence );

	vkWaitForFences( m_device, 1, &m_immediateFence, true, 9999999999 );
}
