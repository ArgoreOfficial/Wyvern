#include "renderer.h"

#include <auxiliary/vk-bootstrap/VkBootstrap.h>

#include <wv/application.h>
#include <wv/debug/log.h>
#include <wv/display_driver.h>
#include <wv/entity/world.h>
#include <wv/rendering/viewport.h>
#include <wv/rendering/swapchain.h>

#include <wv/rendering/components/mesh_component.h>
#include <wv/rendering/systems/render_world_system.h>

///////////////////////////////////////////////////////////////////////////////////////

#ifdef WV_SUPPORT_SDL
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <sdl/sdl_display_driver.h>
#endif

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <tracy/Tracy.hpp>
#include <stdio.h>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////
// Imgui

#include "imgui/imgui.h"

#ifdef WV_SUPPORT_SDL
#include "imgui/backends/imgui_impl_sdl3.h"
#endif

#include "imgui/backends/imgui_impl_vulkan.h"

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

VkBool32 wv::Renderer::debugCallback( 
	VkDebugUtilsMessageSeverityFlagBitsEXT _messageSeverity, 
	VkDebugUtilsMessageTypeFlagsEXT _messageTypes, 
	const VkDebugUtilsMessengerCallbackDataEXT* _pCallbackData, 
	void* _pUserData )
{

	wv::Debug::PrintLevel level = wv::Debug::WV_PRINT_INFO;

	switch ( _messageSeverity )
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: level = wv::Debug::WV_PRINT_DEBUG; break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    level = wv::Debug::WV_PRINT_INFO; break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: level = wv::Debug::WV_PRINT_WARN; break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   level = wv::Debug::WV_PRINT_ERROR; break;
	}

	wv::Renderer* renderer = wv::Application::getSingleton()->getRenderer();

	for ( auto errorInfo : renderer->m_errorInfos )
		if ( errorInfo.first == _pCallbackData->messageIdNumber )
			wv::Debug::Print( wv::Debug::WV_PRINT_DEBUG, "%s\n", errorInfo.second.c_str() );
	
	wv::Debug::Print( level, "%s\n", _pCallbackData->pMessage );
	
	return true;
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

	// Command structures

	m_stagingRing.initialize( m_allocator, 52428800, FRAME_OVERLAP );
	m_commandPoolRing.initialize( m_device, m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, FRAME_OVERLAP );
	m_deleteQueueRing.initialize( FRAME_OVERLAP );

	// Sync structures 

	m_semaphoreRing.initialize( m_device, FRAME_OVERLAP );
	m_fenceRing.initialize( m_device, FRAME_OVERLAP );
	m_fencePool.initialize( m_device );

	if ( !initDescriptors() )
		return false;

	// Create bindless pipeline layout
	{
		VkPushConstantRange pushConstantRange{ };
		pushConstantRange.offset = 0;
		pushConstantRange.size = m_deviceLimits.maxPushConstantsSize;
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

	// Image and sampler stuff

	uint32_t black   = Vector4f{ 0.0f, 0.0f, 0.0f, 1.0f }.packUnorm4x8();
	uint32_t white   = Vector4f{ 1.0f, 1.0f, 1.0f, 1.0f }.packUnorm4x8();
	uint32_t magenta = Vector4f{ 1.0f, 0.0f, 1.0f, 1.0f }.packUnorm4x8();

	std::array<uint32_t, 16 * 16 > pixels;
	for ( int x = 0; x < 16; x++ )
		for ( int y = 0; y < 16; y++ )
			pixels[ y * 16 + x ] = ( ( x % 2 ) ^ ( y % 2 ) ) ? magenta : black;
	
	m_debugImage = m_imageManager.createImage( &pixels, VK_FORMAT_R8G8B8A8_UNORM, VkExtent3D{ 16, 16, 1 }, VK_IMAGE_USAGE_SAMPLED_BIT );

	m_blackImage = m_imageManager.createImage( &black, VK_FORMAT_R8G8B8A8_UNORM, VkExtent3D{ 1, 1, 1 }, VK_IMAGE_USAGE_SAMPLED_BIT );
	m_whiteImage = m_imageManager.createImage( &white, VK_FORMAT_R8G8B8A8_UNORM, VkExtent3D{ 1, 1, 1 }, VK_IMAGE_USAGE_SAMPLED_BIT );

	VkSamplerCreateInfo samplerInfo{ .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.magFilter = VK_FILTER_NEAREST;
	samplerInfo.minFilter = VK_FILTER_NEAREST;
	vkCreateSampler( m_device, &samplerInfo, nullptr, &m_samplerNearest );
	
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	vkCreateSampler( m_device, &samplerInfo, nullptr, &m_samplerLinear );

	m_mainDeleteQueue.push( [ & ]() {
		vkDestroySampler( m_device, m_samplerNearest, nullptr );
		vkDestroySampler( m_device, m_samplerLinear, nullptr );

		m_imageManager.destroyImage( m_debugImage );
		m_imageManager.destroyImage( m_blackImage );
		m_imageManager.destroyImage( m_whiteImage );
	} );

	storeImage( m_debugImage, m_samplerNearest, 0 );
	storeImage( m_blackImage, m_samplerNearest, 1 );
	storeImage( m_whiteImage, m_samplerNearest, 2 );

	m_initialized = true;

	// Debug drawing buffers

	m_debugLineBuffers.initialize( FRAME_OVERLAP );

	// Setup imgui

#ifdef WV_SUPPORT_IMGUI
	{
		std::vector<VkDescriptorPoolSize> poolSizes = { 
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } 
		};

		VkDescriptorPoolCreateInfo poolInfo{ .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000;
		poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();

		VkDescriptorPool imguiPool;
		WV_ASSERT( vkCreateDescriptorPool( m_device, &poolInfo, nullptr, &imguiPool ) == VK_SUCCESS );

		ImGui::CreateContext();

	#ifdef WV_SUPPORT_SDL
		SDLDisplayDriver* sdlDisplayDriver = (SDLDisplayDriver*)displayDriver;
		ImGui_ImplSDL3_InitForVulkan( sdlDisplayDriver->getSDLWindowContext() );
	#else
		error( "No ImGui backend" );
	#endif

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance       = m_instance;
		initInfo.PhysicalDevice = m_physicalDevice;
		initInfo.Device         = m_device;
		initInfo.Queue          = m_graphicsQueue;
		initInfo.DescriptorPool = imguiPool;
		initInfo.MinImageCount = 3;
		initInfo.ImageCount    = 3;
		initInfo.UseDynamicRendering = true;
		initInfo.MinAllocationSize = 1024 * 1024;

		VkFormat swapchainFormat = m_swapchain->getFormat();
		initInfo.PipelineInfoMain.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
		initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchainFormat;

		initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init( &initInfo );

		// ImGui_ImplVulkan_CreateFontsTexture();
		
		// add the destroy the imgui created structures
		m_mainDeleteQueue.push( [ = ]()
								{
									ImGui_ImplVulkan_Shutdown();
									vkDestroyDescriptorPool( m_device, imguiPool, nullptr );
								} );
	}
#endif

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Renderer::shutdown()
{
	if ( m_initialized )
	{
		waitForRenderer();

		for ( auto& b : m_debugLineBuffers.getObjects() )
			destroyBuffer( b );

		m_debugLineBuffers.shutdown();

		m_stagingRing.shutdown();
		m_commandPoolRing.shutdown();

		m_fencePool.shutdown();
		m_fenceRing.shutdown();
		m_semaphoreRing.shutdown();
		
		for ( auto q : m_deleteQueueRing.getObjects() )
			q.flush();

		m_deleteQueueRing.shutdown();

		m_mainDeleteQueue.flush();
		
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void wv::Renderer::render( World* _world )
{
	ZoneScoped;

	std::scoped_lock lock{ m_mtx };

	m_fenceRing.setCycle( m_frameNumber );
	m_semaphoreRing.setCycle( m_frameNumber );
	m_stagingRing.setCycle( m_frameNumber );
	m_commandPoolRing.setCycle( m_frameNumber );

	m_deleteQueueRing.setCycle( m_frameNumber );
	m_deleteQueueRing.get().flush();

	m_debugLineBuffers.setCycle( m_frameNumber );
	
	m_fencePool.resetAvailable();

	uint32_t swapchainImageIndex;
	VkResult e = m_swapchain->acquireNextImage( m_semaphoreRing.getSemaphore(), &swapchainImageIndex);

	if ( e == VK_ERROR_OUT_OF_DATE_KHR )
	{
		m_resizeRequested = true;
		return;
	}

	// Draw

	VkCommandBuffer cmd = m_commandPoolRing.createBuffer( VK_COMMAND_BUFFER_LEVEL_PRIMARY, true );
	// vkResetCommandBuffer( cmd, 0 );

	if ( Viewport* worldViewport = _world->getViewport() )
	{
		if ( ViewVolume* viewVolume = worldViewport->getViewVolume() )
		{
			m_drawExtent.width  = viewVolume->getViewDimensions().x;
			m_drawExtent.height = viewVolume->getViewDimensions().y;
		}
	}
	
	{
		AllocatedImage drawImage  = m_imageManager.getAllocatedImage( m_drawImage );
		AllocatedImage depthImage = m_imageManager.getAllocatedImage( m_depthImage );

		VkImage     swapchainImage     = m_swapchain->getSwapchainImage( swapchainImageIndex );
		VkImageView swapchainImageView = m_swapchain->getSwapchainImageView( swapchainImageIndex );
		VkExtent2D  swapchainExtent    = m_swapchain->getExtent();

		vkCmdBindDescriptorSets( cmd, VK_PIPELINE_BIND_POINT_COMPUTE,  m_bindlessPipelineLayout, 0, 1, &m_bindlessDescriptorSet, 0, nullptr );
		vkCmdBindDescriptorSets( cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_bindlessPipelineLayout, 0, 1, &m_bindlessDescriptorSet, 0, nullptr );

		transitionImage( cmd, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL );

		// draw background colour
		drawBackground( cmd );

		transitionImage( cmd, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
		transitionImage( cmd, depthImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL );
		
		drawGeometry( cmd, _world );
		drawDebug( cmd, _world );
		m_debugLinePositions.clear();

		transitionImage( cmd, drawImage.image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL );
		transitionImage( cmd, swapchainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

		// copy draw to swapchain
		copyImageToImage( cmd, drawImage.image, swapchainImage, m_drawExtent, swapchainExtent );

		transitionImage( cmd, swapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );

	#ifdef WV_DEBUG
	#ifdef WV_SUPPORT_IMGUI
		{
			// Draw ImGui

			beginRendering( cmd, swapchainExtent.width, swapchainExtent.height, swapchainImageView, VK_NULL_HANDLE );
	
			ImGui_ImplVulkan_RenderDrawData( ImGui::GetDrawData(), cmd );
	
			vkCmdEndRendering( cmd );
		}
	#endif
	#endif

		transitionImage( cmd, swapchainImage, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR );

		vkEndCommandBuffer( cmd );
	}

	// Submit

	VkSemaphoreSubmitInfo waitInfo   = semaphoreSubmitInfo( VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, m_semaphoreRing.getSemaphore() );
	VkSemaphoreSubmitInfo signalInfo = semaphoreSubmitInfo( VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, m_swapchain->getSubmitSemaphore( swapchainImageIndex ) );
	submit( cmd, m_graphicsQueue, &waitInfo, &signalInfo, m_fenceRing.getFence() );

	// Present

	pushErrorInfo( -1259592959, "The following error may have been caused by OBS capture. You can safely ignore it." );

	if ( m_swapchain->present( swapchainImageIndex, m_graphicsQueue ) == VK_ERROR_OUT_OF_DATE_KHR )
		m_resizeRequested = true;

	popErrorInfo();

	m_frameNumber++;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::ResourceID wv::Renderer::createPipeline( uint32_t* _vertSrc, uint32_t _vertSize, uint32_t* _fragSrc, uint32_t _fragSize, TopologyClass _topology )
{
	std::scoped_lock lock{ m_mtx };

	VkShaderModule vertShader = m_pipelineManager.createShaderModule( _vertSrc, _vertSize );
	VkShaderModule fragShader = m_pipelineManager.createShaderModule( _fragSrc, _fragSize );

	if ( vertShader == VK_NULL_HANDLE || fragShader == VK_NULL_HANDLE )
	{
		WV_LOG_ERROR( "Failed to create shader modules\n" );
		return {};
	}

	ResourceID pipeline = m_pipelineManager.createGraphicsPipeline( vertShader, fragShader, m_bindlessPipelineLayout, _topology );
	m_pipelineManager.destroyShaderModule( vertShader );
	m_pipelineManager.destroyShaderModule( fragShader );
	
	if ( !pipeline.isValid() )
	{
		WV_LOG_ERROR( "Failed to create pipeline\n" );
		return {};
	}

	return pipeline;
}

void wv::Renderer::destroyPipeline( ResourceID _pipeline )
{
	std::scoped_lock lock{ m_mtx };

	m_pipelineManager.destroyPipeline( _pipeline );
}

wv::ResourceID wv::Renderer::allocateMesh( uint32_t _numIndices, uint32_t _numPositions, uint32_t _vertexDataSize )
{
	std::scoped_lock lock{ m_mtx };

	MeshAllocation meshAllocation{};

	meshAllocation.indexBuffer = createBuffer(
		_numIndices * sizeof( uint16_t ),
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY );

	// create position buffer
	meshAllocation.positionBuffer = createBuffer(
		_numPositions * sizeof( Vector3f ),
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY );
	
	// create vertex data buffer
	if ( _vertexDataSize > 0 )
	{
		meshAllocation.vertexDataBuffer = createBuffer(
			_vertexDataSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VMA_MEMORY_USAGE_GPU_ONLY );

		VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = meshAllocation.vertexDataBuffer.buffer };
		meshAllocation.vertexDataBufferAddress = vkGetBufferDeviceAddress( m_device, &deviceAdressInfo );
	}

	{
		VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = meshAllocation.positionBuffer.buffer };
		meshAllocation.positionBufferAddress = vkGetBufferDeviceAddress( m_device, &deviceAdressInfo );
	}

	return m_meshAllocations.emplace( meshAllocation );
}

void wv::Renderer::deallocateMesh( ResourceID _mesh )
{
	std::scoped_lock lock{ m_mtx };

	if ( !m_meshAllocations.contains( _mesh ) )
		return;

	MeshAllocation surface = m_meshAllocations.at( _mesh );
	m_meshAllocations.erase( _mesh );

	// Push mesh allocation to frame deletion queue

	m_deleteQueueRing.get().push( [ this, surface ]() {
		destroyBuffer( surface.positionBuffer );
		destroyBuffer( surface.indexBuffer );

		if ( surface.vertexDataBuffer.buffer != VK_NULL_HANDLE )
			destroyBuffer( surface.vertexDataBuffer );
	} );
}

void wv::Renderer::uploadMesh( ResourceID _mesh, const uint16_t* _indices, const Vector3f* _positions, const void* _vertexData )
{
	std::scoped_lock lock{ m_mtx };

	if ( !m_meshAllocations.contains( _mesh ) )
	{
		WV_LOG_ERROR( "Mesh does not exist\n" );
		return;
	}

	MeshAllocation meshAllocation = m_meshAllocations.at( _mesh );

	const VkBuffer indexBuffer      = meshAllocation.indexBuffer.buffer;
	const VkBuffer positionBuffer   = meshAllocation.positionBuffer.buffer;
	const VkBuffer vertexDataBuffer = meshAllocation.vertexDataBuffer.buffer;
	
	const VkDeviceSize indexBufferSize    = meshAllocation.indexBuffer.actualSize;
	const VkDeviceSize positionBufferSize = meshAllocation.positionBuffer.actualSize;
	const VkDeviceSize vertexDataSize     = meshAllocation.vertexDataBuffer.actualSize;
	
	// Upload buffers

	VkDeviceSize bufferSize = positionBufferSize + indexBufferSize + vertexDataSize;
	VirtualAllocSpan staging = m_stagingRing.allocate( bufferSize );
	WV_ASSERT( staging.size == bufferSize );

	memcpy( staging.mapping, _positions, positionBufferSize ); // copy position buffer
	memcpy( (char*)staging.mapping + positionBufferSize, _indices, indexBufferSize ); // copy index buffer

	if ( _vertexData != nullptr )
		memcpy( (char*)staging.mapping + positionBufferSize + indexBufferSize, _vertexData, vertexDataSize ); // copy vertex data buffer

	immediateCmdSubmit( [=]( VkCommandBuffer _cmd ) {
		VkBufferCopy positionCopy{ 0 };
		positionCopy.dstOffset = 0;
		positionCopy.srcOffset = staging.offset;
		positionCopy.size = positionBufferSize;

		vkCmdCopyBuffer( _cmd, staging.buffer, positionBuffer, 1, &positionCopy );

		VkBufferCopy indexCopy{ 0 };
		indexCopy.dstOffset = 0;
		indexCopy.srcOffset = staging.offset + positionBufferSize;
		indexCopy.size = indexBufferSize;

		vkCmdCopyBuffer( _cmd, staging.buffer, indexBuffer, 1, &indexCopy );

		if ( _vertexData != nullptr )
		{
			VkBufferCopy vertexDataCopy{ 0 };
			vertexDataCopy.dstOffset = 0;
			vertexDataCopy.srcOffset = staging.offset + positionBufferSize + indexBufferSize;
			vertexDataCopy.size = vertexDataSize;

			vkCmdCopyBuffer( _cmd, staging.buffer, vertexDataBuffer, 1, &vertexDataCopy );
		}
	} );

}

wv::ResourceID wv::Renderer::allocateImage( const void* _data, int _width, int _height, bool _mipmapped )
{
	std::scoped_lock lock{ m_mtx };

	VkExtent3D imagesize;
	imagesize.width = _width;
	imagesize.height = _height;
	imagesize.depth = 1;
	
	return m_imageManager.createImage( _data, VK_FORMAT_R8G8B8A8_UNORM, imagesize, VK_IMAGE_USAGE_SAMPLED_BIT, _mipmapped );
}

void wv::Renderer::deallocateImage( ResourceID _image )
{
	std::scoped_lock lock{ m_mtx };

	if ( m_storedImageIndexMap.contains( _image ) )
	{
		m_storedImageIndices.erase( m_storedImageIndexMap.at( _image ) );
		m_storedImageIndexMap.erase( _image );
	}

	m_deleteQueueRing.get().push( [ this, _image ]() {
		m_imageManager.destroyImage( _image );
	} );
	
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initVulkan()
{
	vkb::InstanceBuilder builder;
	auto ret = builder.set_app_name( "Wyvern" )
		.request_validation_layers( m_useValidationLayers )
		.set_debug_callback( debugCallback )
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

#ifdef WV_SUPPORT_SDL
	{
		SDLDisplayDriver* sdlDisplayDriver = static_cast<SDLDisplayDriver*>( displayDriver );
		SDL_Vulkan_CreateSurface( sdlDisplayDriver->getSDLWindowContext(), m_instance, nullptr, &m_surface );
	}
#endif

	VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
	features.dynamicRendering = true;
	features.synchronization2 = true;
	
	VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	features12.bufferDeviceAddress    = true;
	features12.descriptorIndexing     = true;
	features12.runtimeDescriptorArray = true;

	vkb::PhysicalDeviceSelector selector{ vkbInstance };
	vkb::PhysicalDevice physicalDevice = selector
		.set_minimum_version( 1, 3 )
		.set_required_features_13( features )
		.set_required_features_12( features12 )
		.set_surface( m_surface )
		.select()
		.value();

	m_deviceLimits = physicalDevice.properties.limits;

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
	m_swapchain = WV_NEW( Swapchain, m_device, m_physicalDevice, m_surface );
	m_swapchain->createSwapchain( _width, _height );

	m_mainDeleteQueue.push( [ & ]() {
		m_swapchain->destroySwapchain();
		WV_FREE( m_swapchain );
	} );

	Vector2i displaySize = Application::getSingleton()->getDisplayDriver()->getDisplaySize();
	VkExtent3D drawImageExtent = { (uint32_t)displaySize.x, (uint32_t)displaySize.y, 1 };

	// Create draw image
	m_drawImage = m_imageManager.createImage(
		VK_FORMAT_R16G16B16A16_SFLOAT, 
		drawImageExtent, 
		VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT 
	);

	// Create depth image
	m_depthImage = m_imageManager.createImage(
		VK_FORMAT_D32_SFLOAT, 
		drawImageExtent, 
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
	);

	m_mainDeleteQueue.push( [ & ]() {
		m_imageManager.destroyImage( m_drawImage );
		m_imageManager.destroyImage( m_depthImage );
	} );


	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initSyncStructures()
{
	
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

void wv::Renderer::resizeSwapchain( uint32_t _width, uint32_t _height )
{
	std::scoped_lock lock{ m_mtx };

	if ( _width == 0 || _height == 0 )
		return;

	m_swapchain->recreateSwapchain( _width, _height );

	m_resizeRequested = false;
}

void wv::Renderer::beginDebugRender()
{
#ifdef WV_SUPPORT_IMGUI
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();
#endif
}

void wv::Renderer::endDebugRender()
{
#ifdef WV_SUPPORT_IMGUI
	ImGui::Render();
#endif
}

void wv::Renderer::drawBackground( VkCommandBuffer _cmd )
{
	ZoneScoped;

	//float flash = std::abs( std::sin( m_frameNumber / 120.f ) );
	//VkClearColorValue clearValue = { { 0.0f, 0.0f, flash, 1.0f } };
	VkClearColorValue clearValue = { { 0.1f, 0.1f, 0.1f, 0.0f } };

	AllocatedImage image = m_imageManager.getAllocatedImage( m_drawImage );
	VkImageSubresourceRange clearRange = makeVkImageSubresourceRange( VK_IMAGE_ASPECT_COLOR_BIT );
	vkCmdClearColorImage( _cmd, image.image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange );
}

void wv::Renderer::drawGeometry( VkCommandBuffer _cmd, World* _world )
{ 
	ZoneScoped;

	RenderWorldSystem* worldRenderSystem = _world->getWorldSystem<RenderWorldSystem>();
	if ( !worldRenderSystem )
		return;

	Viewport* worldViewport = _world->getViewport();
	if ( !worldViewport )
		return;

	ViewVolume* viewVolume = worldViewport->getViewVolume();
	if ( !viewVolume )
		return;

	AllocatedImage drawImage = m_imageManager.getAllocatedImage( m_drawImage );
	AllocatedImage depthImage = m_imageManager.getAllocatedImage( m_depthImage );

	beginRendering( _cmd, m_drawExtent.width, m_drawExtent.height, drawImage.imageView, depthImage.imageView );
	
	// set default render state
	{
		VkViewport viewport = makeVkViewport( 0, 0, m_drawExtent.width, m_drawExtent.height );
		vkCmdSetViewport( _cmd, 0, 1, &viewport );

		VkRect2D scissor = makeVkRect2D( 0, 0, m_drawExtent.width, m_drawExtent.height );
		vkCmdSetScissor( _cmd, 0, 1, &scissor );
	
		vkCmdSetDepthTestEnable ( _cmd, VK_TRUE );
		vkCmdSetDepthWriteEnable( _cmd, VK_TRUE );
		vkCmdSetDepthCompareOp  ( _cmd, VK_COMPARE_OP_GREATER_OR_EQUAL );
	
		vkCmdSetStencilTestEnable( _cmd, VK_FALSE );

		vkCmdSetPrimitiveTopology( _cmd, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );
	}

	Matrix4x4f viewProj = viewVolume->getViewProjMatrix();
			
	auto buckets = worldRenderSystem->getRenderBuckets();

	for ( auto bucket : buckets )
	{
		for ( size_t i = 0; i < bucket.renderMeshes.size(); i++ )
		{
			RenderMesh& renderMesh = bucket.renderMeshes[ i ];
			ResourceID meshHandle = bucket.renderMeshes[ i ].mesh;
			if ( !meshHandle.isValid() )
				continue; // no allocated mesh
			if ( !renderMesh.pipeline.isValid() )
				continue; // no material
			
			const MeshAllocation& mesh = m_meshAllocations.at( meshHandle );

			WV_ASSERT( mesh.indexBuffer.buffer != VK_NULL_HANDLE );
			vkCmdBindIndexBuffer( _cmd, mesh.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16 );

			Pipeline pipeline = m_pipelineManager.getPipeline( renderMesh.pipeline );

			WV_ASSERT( pipeline.pipeline != VK_NULL_HANDLE );
			vkCmdBindPipeline( _cmd, pipeline.bindPoint, pipeline.pipeline );

			GPUDrawPushConstants pc{};
			pc.viewProj = viewProj;
			pc.model = bucket.matrices[ i ];
			pc.positionBuffer = mesh.positionBufferAddress;

			if ( mesh.vertexDataBuffer.buffer != VK_NULL_HANDLE )
				pc.vertexDataBuffer = mesh.vertexDataBufferAddress;

			vkCmdPushConstants(
				_cmd,
				m_bindlessPipelineLayout,
				VK_SHADER_STAGE_ALL, 0,
				sizeof( GPUDrawPushConstants ),
				&pc
			);

			if ( renderMesh.materialData.size() > 0 )
			{
				vkCmdPushConstants( 
					_cmd, 
					m_bindlessPipelineLayout,
					VK_SHADER_STAGE_ALL, sizeof( GPUDrawPushConstants ),
					renderMesh.materialData.size(),
					renderMesh.materialData.data()
				);
			}

			vkCmdDrawIndexed( _cmd, renderMesh.indexCount, 1, renderMesh.firstIndex, renderMesh.vertexOffset, 0 );
		}
	}
	
	vkCmdEndRendering( _cmd );
}

void wv::Renderer::drawDebug( VkCommandBuffer _cmd, World* _world )
{
	size_t requiredDebugBufferSize = m_debugLinePositions.size() * sizeof( wv::Vector3f );

	if ( requiredDebugBufferSize <= 0 )
		return;

	AllocatedBuffer& debugBuffer = m_debugLineBuffers.get();

	//if ( requiredDebugBufferSize > 65536 )
	//	return; // too much data, should throw error

	// buffer resize

	if ( debugBuffer.actualSize < requiredDebugBufferSize )
	{
		destroyBuffer( debugBuffer );
		debugBuffer = createBuffer(
			requiredDebugBufferSize,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
			VMA_MEMORY_USAGE_CPU_TO_GPU );
	}

	// update buffer
	
	memcpy( debugBuffer.allocation->GetMappedData(), m_debugLinePositions.data(), requiredDebugBufferSize );

	/*
	vkCmdUpdateBuffer( _cmd, debugBuffer.buffer, 0, requiredDebugBufferSize, m_debugLinePositions.data() );
	
	VkMemoryBarrier2 memoryBarrier{ .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 };
	memoryBarrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
	memoryBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	memoryBarrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT;
	memoryBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	VkDependencyInfo depInfo{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
	depInfo.memoryBarrierCount = 1;
	depInfo.pMemoryBarriers = &memoryBarrier;

	vkCmdPipelineBarrier2( _cmd, &depInfo );
	*/

	// bind and draw

	Viewport* worldViewport = _world->getViewport();
	if ( !worldViewport )
		return;

	ViewVolume* viewVolume = worldViewport->getViewVolume();
	if ( !viewVolume )
		return;

	AllocatedImage drawImage = m_imageManager.getAllocatedImage( m_drawImage );
	AllocatedImage depthImage = m_imageManager.getAllocatedImage( m_depthImage );

	beginRendering( _cmd, m_drawExtent.width, m_drawExtent.height, drawImage.imageView, depthImage.imageView );

	// set default render state
	{
		VkViewport viewport = makeVkViewport( 0, 0, m_drawExtent.width, m_drawExtent.height );
		vkCmdSetViewport( _cmd, 0, 1, &viewport );

		VkRect2D scissor = makeVkRect2D( 0, 0, m_drawExtent.width, m_drawExtent.height );
		vkCmdSetScissor( _cmd, 0, 1, &scissor );

		vkCmdSetDepthTestEnable( _cmd, VK_FALSE );
		vkCmdSetDepthWriteEnable( _cmd, VK_FALSE );
		vkCmdSetDepthCompareOp( _cmd, VK_COMPARE_OP_NEVER );

		vkCmdSetStencilTestEnable( _cmd, VK_FALSE );

		vkCmdSetPrimitiveTopology( _cmd, VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
	}

	Matrix4x4f viewProj = viewVolume->getViewProjMatrix();

	MaterialInstance debugMaterial = _world->getMaterialManager()->get( "Debug" );
	debugMaterial.setValue( "color", wv::Vector4f{ 1.f, 0.f, 1.f, 1.f } );

	Pipeline pipeline = m_pipelineManager.getPipeline( debugMaterial.material->getPipeline() );

	WV_ASSERT( pipeline.pipeline != VK_NULL_HANDLE );
	vkCmdBindPipeline( _cmd, pipeline.bindPoint, pipeline.pipeline );

	GPUDrawPushConstants pc{};
	pc.viewProj = viewProj;
	pc.model = wv::Matrix4x4f::identity( 1.0 );
	pc.positionBuffer = debugBuffer.deviceAddress;

	vkCmdPushConstants(
		_cmd,
		m_bindlessPipelineLayout,
		VK_SHADER_STAGE_ALL, 0,
		sizeof( GPUDrawPushConstants ),
		&pc
	);

	vkCmdPushConstants(
		_cmd,
		m_bindlessPipelineLayout,
		VK_SHADER_STAGE_ALL, sizeof( GPUDrawPushConstants ),
		debugMaterial.buffer.size(),
		debugMaterial.buffer.data()
	);

	vkCmdDraw( _cmd, m_debugLinePositions.size(), 1, 0, 0 );

	//vkCmdDrawIndexed( _cmd, renderMesh.indexCount, 1, renderMesh.firstIndex, renderMesh.vertexOffset, 0 );
	
	vkCmdEndRendering( _cmd );
}


void wv::Renderer::immediateCmdSubmit( std::function<void( VkCommandBuffer _cmd )>&& _func )
{
	ZoneScoped;

	std::scoped_lock lock{ m_mtx };

	VkFence fence = m_fencePool.getFence();
	
	VkCommandBuffer cmd = m_commandPoolRing.createBuffer( VK_COMMAND_BUFFER_LEVEL_PRIMARY, true );
	_func( cmd );
	vkEndCommandBuffer( cmd );

	submit( cmd, m_graphicsQueue, nullptr, nullptr, fence );
	
	vkWaitForFences( m_device, 1, &fence, true, 9999999999 );
}

wv::AllocatedBuffer wv::Renderer::createBuffer( size_t _size, VkBufferUsageFlags _usage, VmaMemoryUsage _memoryUsage )
{
	ZoneScoped;

	std::scoped_lock lock{ m_mtx };

	VkBufferCreateInfo bufferInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	bufferInfo.size = _size;
	bufferInfo.usage = _usage;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = _memoryUsage;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	AllocatedBuffer buffer{};
	WV_ASSERT( vmaCreateBuffer( m_allocator, &bufferInfo, &allocInfo, &buffer.buffer, &buffer.allocation, &buffer.info ) == VK_SUCCESS );

	buffer.actualSize = _size;

	if ( _usage & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT )
	{
		VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = buffer.buffer };
		buffer.deviceAddress = vkGetBufferDeviceAddress( m_device, &deviceAdressInfo );
	}

	return buffer;
}

void wv::Renderer::destroyBuffer( const AllocatedBuffer& _buffer )
{
	ZoneScoped;

	std::scoped_lock lock{ m_mtx };

	vmaDestroyBuffer( m_allocator, _buffer.buffer, _buffer.allocation );
}

void wv::Renderer::storeImage( ResourceID _imageID, VkSampler _sampler, uint32_t _at )
{
	ZoneScoped;

	std::scoped_lock lock{ m_mtx };

	AllocatedImage image = m_imageManager.getAllocatedImage( _imageID );
	
	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = image.imageView;
	imageInfo.sampler = _sampler;

	VkWriteDescriptorSet write{ .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
	write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.dstBinding = SAMPLER_BINDING;
	write.dstSet = m_bindlessDescriptorSet;

	write.descriptorCount = 1;

	write.dstArrayElement = _at; // element index
	write.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets( m_device, 1, &write, 0, nullptr );

	m_storedImageIndices.insert( _at );
	m_storedImageIndexMap.emplace( _imageID, _at );
}

wv::VirtualAllocSpan wv::StagingBufferRing::allocate( VkDeviceSize _size )
{
	std::scoped_lock lock{ m_mtx };

	VirtualAllocSpan span{};
	VkResult res = VK_ERROR_UNKNOWN;
	
	for ( size_t i = 0; i < m_stagingBuffers.size(); i++ )
	{
		res = tryAllocateSpan( _size, i, span );

		if( res == VK_SUCCESS )
			break;
	}

	if ( res != VK_SUCCESS )
	{
		allocateStaging( _size );
		WV_ASSERT( tryAllocateSpan( _size, m_stagingBuffers.size() - 1, span ) == VK_SUCCESS );
	}

	getAllocVec( m_cycleIndex ).push_back( span );

	return span;
}

VkResult wv::StagingBufferRing::tryAllocateSpan( VkDeviceSize _size, size_t _blockIndex, VirtualAllocSpan& _outSpan )
{
	VmaVirtualAllocationCreateInfo allocInfo{};
	allocInfo.size = _size;

	StagingBufferAllocation& buffer = m_stagingBuffers[ _blockIndex ];
	VkResult res = vmaVirtualAllocate( buffer.virtualBlock, &allocInfo, &_outSpan.alloc, &_outSpan.offset );
	if ( res != VK_SUCCESS )
		return res;

	_outSpan.mapping = (char*)buffer.allocation->GetMappedData() + _outSpan.offset;
	_outSpan.buffer = buffer.buffer;
	_outSpan.virtualBlockIndex = _blockIndex;
	_outSpan.size = _size;
	return res;
}
