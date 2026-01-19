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

	IFileSystem* fileSystem = wv::Application::getSingleton()->getFileSystem();


	// Create bindless pipeline layout
	{
		VkPushConstantRange pushConstantRange{ };
		pushConstantRange.offset = 0;
		pushConstantRange.size = 128;
		pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		pipelineLayoutInfo.pSetLayouts = &m_bindlessLayout;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		pipelineLayoutInfo.pushConstantRangeCount = 1;

		vkCreatePipelineLayout( m_device, &pipelineLayoutInfo, nullptr, &m_bindlessPipelineLayout );

		m_mainDeleteQueue.push( [ & ]() {
			vkDestroyPipelineLayout( m_device, m_bindlessPipelineLayout, nullptr );
		} );
	}

	//{
	//	
	//	VkDescriptorBufferInfo bufInfo{};
	//	
	//	VkDescriptorImageInfo imgInfo{};
	//	imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	//	imgInfo.imageView = m_drawImage.imageView;

	//	VkWriteDescriptorSet write{ .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	//	write.dstBinding = 0;
	//	write.dstSet = m_bindlessDescriptorSet;
	//	write.descriptorCount = 1;
	//	write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	//	write.pImageInfo = &imgInfo;
	//	vkUpdateDescriptorSets( m_device, 1, &write, 0, nullptr );

	//}



	// Triangle

	// Create triangle pipline
	{
		std::vector<uint8_t> vertShaderData = fileSystem->loadEntireFile( "shaders/coloured_triangle.vert.spv" );
		std::vector<uint8_t> fragShaderData = fileSystem->loadEntireFile( "shaders/coloured_triangle.frag.spv" );
		VkShaderModule vertShader = m_pipelineManager.createShaderModule( (uint32_t*)vertShaderData.data(), vertShaderData.size() );
		VkShaderModule fragShader = m_pipelineManager.createShaderModule( (uint32_t*)fragShaderData.data(), fragShaderData.size() );
		
		if ( vertShader == VK_NULL_HANDLE || fragShader == VK_NULL_HANDLE )
			return false;

		m_trianglePipelineID = m_pipelineManager.createGraphicsPipeline( vertShader, fragShader, m_bindlessPipelineLayout );
		m_pipelineManager.destroyShaderModule( vertShader );
		m_pipelineManager.destroyShaderModule( fragShader );

		m_mainDeleteQueue.push( [ & ]() {
			m_pipelineManager.destroyPipeline( m_trianglePipelineID );
		} );
	}

	// Create triangle
	{
		m_triangleBuffers = uploadMesh(
			{ 0, 1, 2 },
			{ 
				Vector3f( 0.5f, 0.5f, 0.0f ),
				Vector3f(-0.5f, 0.5f, 0.0f),
				Vector3f( 0.0f,-0.5f, 0.0f)
			} 
		);

		m_mainDeleteQueue.push( [ & ]() {
			destroyMesh( m_triangleBuffers );
			m_triangleBuffers = {};
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

		cmd->bindDescriptorSets( VK_PIPELINE_BIND_POINT_COMPUTE,  m_bindlessPipelineLayout, 0, 1, &m_bindlessDescriptorSet );
		cmd->bindDescriptorSets( VK_PIPELINE_BIND_POINT_GRAPHICS, m_bindlessPipelineLayout, 0, 1, &m_bindlessDescriptorSet );

		cmd->transitionImage( m_drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL );

		// draw background colour
		drawBackground( cmd );

		cmd->transitionImage( m_drawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		
		drawGeometry( cmd );

		cmd->transitionImage( m_drawImage.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );
		cmd->transitionImage( m_swapchainImages[ swapchainImageIndex ], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

		// copy draw to swapchain
		cmd->copyImageToImage( m_drawImage.image, m_swapchainImages[ swapchainImageIndex ], m_drawExtent, m_swapchainExtent );

		cmd->transitionImage( m_swapchainImages[ swapchainImageIndex ], VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );

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
	std::vector<VkDescriptorPoolSize> sizes =
	{
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         STORAGE_COUNT },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SAMPLER_COUNT },
		VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          IMAGE_COUNT }
	};

	VkDescriptorPoolCreateInfo poolCreateInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
	poolCreateInfo.maxSets = 1;
	poolCreateInfo.poolSizeCount = (uint32_t)sizes.size();
	poolCreateInfo.pPoolSizes    = sizes.data();

	vkCreateDescriptorPool( m_device, &poolCreateInfo, nullptr, &m_bindlessPool );


	std::vector<VkDescriptorSetLayoutBinding> bindings = {
		VkDescriptorSetLayoutBinding{
			.binding = STORAGE_BINDING,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			.descriptorCount = STORAGE_COUNT,
			.stageFlags = VK_SHADER_STAGE_ALL
		},
		VkDescriptorSetLayoutBinding{
			.binding = SAMPLER_BINDING,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = SAMPLER_COUNT,
			.stageFlags = VK_SHADER_STAGE_ALL
		},
		VkDescriptorSetLayoutBinding{
			.binding = IMAGE_BINDING,
			.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
			.descriptorCount = IMAGE_COUNT,
			.stageFlags = VK_SHADER_STAGE_ALL
		}
	};
	
	std::vector<VkDescriptorBindingFlags> bindingFlags = {
		VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
		VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
		VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT,
	};
	
	VkDescriptorSetLayoutBindingFlagsCreateInfo layoutBindingFlagsInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO };
	layoutBindingFlagsInfo.pBindingFlags = bindingFlags.data();
	layoutBindingFlagsInfo.bindingCount  = (uint32_t)bindingFlags.size();

	VkDescriptorSetLayoutCreateInfo layoutCreateInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	layoutCreateInfo.pBindings = bindings.data();
	layoutCreateInfo.bindingCount = (uint32_t)bindings.size();
	layoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

	vkCreateDescriptorSetLayout( m_device, &layoutCreateInfo, nullptr, &m_bindlessLayout );

	VkDescriptorSetAllocateInfo allocInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocInfo.descriptorPool     = m_bindlessPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts        = &m_bindlessLayout;

	vkAllocateDescriptorSets( m_device, &allocInfo, &m_bindlessDescriptorSet );

	m_mainDeleteQueue.push( [ & ]() {
		vkDestroyDescriptorPool( m_device, m_bindlessPool, nullptr );
		vkDestroyDescriptorSetLayout( m_device, m_bindlessLayout, nullptr );
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
	float flash = std::abs( std::sin( m_frameNumber / 120.f ) );
	VkClearColorValue clearValue = { { 0.0f, 0.0f, flash, 1.0f } };

	_cmd->clearColorImage( m_drawImage.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue );
}

void wv::Renderer::drawGeometry( CommandBuffer* _cmd )
{ 
	VkRenderingAttachmentInfo colorAttachment{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
	colorAttachment.imageView = m_drawImage.imageView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//if ( clear ) colorAttachment.clearValue = *clear;

	VkRenderingInfo renderInfo{ .sType = VK_STRUCTURE_TYPE_RENDERING_INFO };
	renderInfo.renderArea = VkRect2D{ VkOffset2D { 0, 0 }, m_drawExtent };
	renderInfo.layerCount = 1;
	renderInfo.colorAttachmentCount = 1;
	renderInfo.pColorAttachments = &colorAttachment;
	renderInfo.pDepthAttachment = nullptr;
	renderInfo.pStencilAttachment = nullptr;

	vkCmdBeginRendering( _cmd->m_cmd, &renderInfo );

	Pipeline pipeline = m_pipelineManager.getPipeline( m_trianglePipelineID );
	_cmd->bindPipeline( pipeline.bindPoint, pipeline.pipeline );

	{
		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = 0;
		viewport.width    = m_drawExtent.width;
		viewport.height   = m_drawExtent.height;
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;

		vkCmdSetViewport( _cmd->m_cmd, 0, 1, &viewport );

		VkRect2D scissor = {};
		scissor.offset.x      = 0;
		scissor.offset.y      = 0;
		scissor.extent.width  = m_drawExtent.width;
		scissor.extent.height = m_drawExtent.height;

		vkCmdSetScissor( _cmd->m_cmd, 0, 1, &scissor );


		GPUDrawPushConstants pc{};
		pc.worldMatrix = Matrix4x4f::identity( 1.0f );
		pc.vertexBuffer = m_triangleBuffers.vertexBufferAddress;

		vkCmdBindIndexBuffer( _cmd->m_cmd, m_triangleBuffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16 );

		vkCmdPushConstants( _cmd->m_cmd, m_bindlessPipelineLayout, VK_SHADER_STAGE_ALL, 0, sizeof( GPUDrawPushConstants ), &pc );

		// launch a draw command to draw 3 vertices
		// vkCmdDraw( _cmd->m_cmd, 3, 1, 0, 0 );
		vkCmdDrawIndexed( _cmd->m_cmd, 3, 1, 0, 0, 0 );
	}
	
	vkCmdEndRendering( _cmd->m_cmd );
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

wv::AllocatedBuffer wv::Renderer::createBuffer( size_t _size, VkBufferUsageFlags _usage, VmaMemoryUsage _memoryUsage )
{
	VkBufferCreateInfo bufferInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = _size;
	bufferInfo.usage = _usage;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = _memoryUsage;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	AllocatedBuffer buffer{};
	vmaCreateBuffer( m_allocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, &buffer.info );

	return buffer;
}

void wv::Renderer::destroyBuffer( const AllocatedBuffer& _buffer )
{ 
	vmaDestroyBuffer( m_allocator, _buffer.buffer, _buffer.allocation );
}

wv::GPUMeshBuffers wv::Renderer::uploadMesh( const std::vector<uint16_t>& _indices, const std::vector<Vector3f>& _vertexPositions )
{
	const size_t indexBufferSize  = _indices.size() * sizeof( uint16_t );
	const size_t vertexBufferSize = _vertexPositions.size() * sizeof( Vector3f );

	GPUMeshBuffers surface{};

	surface.indexBuffer = createBuffer( 
		indexBufferSize, 
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
		VMA_MEMORY_USAGE_GPU_ONLY );

	//create vertex buffer
	surface.vertexBuffer = createBuffer( 
		vertexBufferSize, 
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY );

	//find the adress of the vertex buffer
	VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,.buffer = surface.vertexBuffer.buffer };
	surface.vertexBufferAddress = vkGetBufferDeviceAddress( m_device, &deviceAdressInfo );

	// Upload buffers

	AllocatedBuffer staging = createBuffer( vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY );

	void* data = staging.allocation->GetMappedData();
	// copy vertex buffer
	memcpy( data, _vertexPositions.data(), vertexBufferSize );
	// copy index buffer
	memcpy( (char*)data + vertexBufferSize, _indices.data(), indexBufferSize );

	immediateCmdSubmit( [ & ]( CommandBuffer& _cmd ) {
		VkBufferCopy vertexCopy{ 0 };
		vertexCopy.dstOffset = 0;
		vertexCopy.srcOffset = 0;
		vertexCopy.size = vertexBufferSize;

		vkCmdCopyBuffer( _cmd.m_cmd, staging.buffer, surface.vertexBuffer.buffer, 1, &vertexCopy );

		VkBufferCopy indexCopy{ 0 };
		indexCopy.dstOffset = 0;
		indexCopy.srcOffset = vertexBufferSize;
		indexCopy.size = indexBufferSize;

		vkCmdCopyBuffer( _cmd.m_cmd, staging.buffer, surface.indexBuffer.buffer, 1, &indexCopy );
	} );

	destroyBuffer( staging );

	return surface;
}

void wv::Renderer::destroyMesh( const GPUMeshBuffers& _mesh )
{ 
	destroyBuffer( _mesh.vertexBuffer );
	destroyBuffer( _mesh.indexBuffer );
}
