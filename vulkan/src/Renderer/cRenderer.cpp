#include "cRenderer.h"

#include "../Window/cWindow.h"

#include <fstream>
#include <set>
#include <vector>
#include <string>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////////

/* TODO: remove this? */
#define VECTOR_VK_GET( _vec, _func, ... )            \
uint32_t num_##_func;                                \
_func( __VA_ARGS__, &num_##_func, nullptr );         \
if ( num_##_func != 0 )                              \
{                                                    \
	_vec.resize( num_##_func );                      \
	_func( __VA_ARGS__, &num_##_func, _vec.data() ); \
}                                                    \

///////////////////////////////////////////////////////////////////////////////////////

/* just copy pasted lmao idk how this works */
VkResult CreateDebugUtilsMessengerEXT( VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * pCreateInfo, const VkAllocationCallbacks * pAllocator, VkDebugUtilsMessengerEXT * pDebugMessenger )
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
	if ( func != nullptr )
	{
		return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator )
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
	if ( func != nullptr )
	{
		func( instance, debugMessenger, pAllocator );
	}
}

///////////////////////////////////////////////////////////////////////////////////////

// TODO: change to wv::memory and put in content manager or wv::filesystem
static std::vector<char> readFile( const std::string& _filename )
{
	std::ifstream file( _filename, std::ios::ate | std::ios::binary );

	if ( !file.is_open() )
	{
		printf( "[ERROR] Failed to open file %s.\n", _filename.c_str() );
		return {};
	}

	size_t size = (size_t)file.tellg();
	std::vector<char> buffer( size );

	file.seekg( 0 );
	file.read( buffer.data(), size );
	file.close();

	return buffer;
}

///////////////////////////////////////////////////////////////////////////////////////

cm::cRenderer::cRenderer()
{
}

///////////////////////////////////////////////////////////////////////////////////////

cm::cRenderer::~cRenderer( void )
{
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::init( cWindow* _window )
{
	m_active_window = _window;

	createInstance();
	setupDebugMessenger();
	createSurface();
	selectPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFramebuffers();
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::destroy( void )
{
	vkDeviceWaitIdle( m_device );
	
	for ( int i = 0; i < m_max_frames_in_flight; i++ )
	{
		vkDestroySemaphore( m_device, m_image_available_semaphores[ i ], nullptr );
		vkDestroySemaphore( m_device, m_render_finished_semaphores[ i ], nullptr );
		vkDestroyFence    ( m_device, m_in_flight_fences[ i ], nullptr );
	}

	vkDestroyCommandPool( m_device, m_command_pool, nullptr );
	
	vkDestroyPipeline( m_device, m_graphics_pipeline, nullptr );
	vkDestroyPipelineLayout( m_device, m_pipeline_layout, nullptr );
	vkDestroyRenderPass( m_device, m_render_pass, nullptr );
	
	cleanupSwapchain();
	 
	vkDestroyDevice( m_device, nullptr );

	vkDestroySurfaceKHR( m_instance, m_surface, nullptr );

	if ( m_enable_validation_layers )
		DestroyDebugUtilsMessengerEXT( m_instance, m_debug_messenger, nullptr );
	
	vkDestroyInstance( m_instance, nullptr );
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::draw( void )
{
	vkWaitForFences( m_device, 1, &m_in_flight_fences[ m_current_frame ], VK_TRUE, UINT64_MAX);
	
	uint32_t image_index;
	VkResult image_result = vkAcquireNextImageKHR( m_device, m_swapchain, UINT64_MAX, m_image_available_semaphores[ m_current_frame ], VK_NULL_HANDLE, &image_index );

	if ( image_result == VK_ERROR_OUT_OF_DATE_KHR )
	{
		recreateSwapchain();
		return;
	}
	else if ( image_result != VK_SUCCESS && image_result != VK_SUBOPTIMAL_KHR )
		printErrorResult( "[FATAL] Failed to acquire swap chain image:", image_result );
	
	vkResetFences( m_device, 1, &m_in_flight_fences[ m_current_frame ] );

	vkResetCommandBuffer( m_command_buffers[ m_current_frame ], 0 );
	recordCommandBuffer( m_command_buffers[ m_current_frame ], image_index );

	VkSemaphore wait_semaphores  [] = { m_image_available_semaphores[ m_current_frame ] };
	VkSemaphore signal_semaphores[] = { m_render_finished_semaphores[ m_current_frame ] };
	VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	
	VkSubmitInfo submit_info{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount   = 1,
		.pWaitSemaphores      = wait_semaphores,
		.pWaitDstStageMask    = wait_stages,
		.commandBufferCount   = 1,
		.pCommandBuffers      = &m_command_buffers[ m_current_frame ],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores    = signal_semaphores
	};

	const VkResult submit_result = vkQueueSubmit( m_graphics_queue, 1, &submit_info, m_in_flight_fences[ m_current_frame ] );
	if ( submit_result != VK_SUCCESS )
		printErrorResult( "[FATAL] Failed to submit Command Buffer:", submit_result );
	
	/* presenting */
	VkSwapchainKHR swapchains[] = { m_swapchain };

	VkPresentInfoKHR present_info{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores    = signal_semaphores,
		.swapchainCount     = 1,
		.pSwapchains        = swapchains,
		.pImageIndices      = &image_index
	};

	const VkResult present_result = vkQueuePresentKHR( m_present_queue, &present_info );
	if ( present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR )
		recreateSwapchain();
	else if ( present_result != VK_SUCCESS )
		printErrorResult( "[FATAL] Failed to present swap chain image:", image_result );
	
	m_current_frame = ( m_current_frame + 1 ) % m_max_frames_in_flight;
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::recreateSwapchain()
{
	vkDeviceWaitIdle( m_device );

	cleanupSwapchain();

	createSwapChain();
	createImageViews();
	createFramebuffers();
}

///////////////////////////////////////////////////////////////////////////////////////

VKAPI_ATTR VkBool32 VKAPI_CALL cm::cRenderer::debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT _severity, VkDebugUtilsMessageTypeFlagsEXT _type, const VkDebugUtilsMessengerCallbackDataEXT* _data, void* _user_data )
{
	switch ( _severity )
	{
	//case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: printf( "[DIAGNOSTIC] " ); break;
	//case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    printf( "[INFO] " );       break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: printf( "[WARN] " );       break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   printf( "[ERROR] " );      break;
	
	default: return VK_FALSE; break;
	}

	std::string str( _data->pMessage );
	std::replace( str.begin(), str.end(), '|', '\n' );

	printf( "%s\n\n", str.c_str() );
	return VK_FALSE;	
}

void cm::cRenderer::createInstance( void )
{
	if ( m_enable_validation_layers && !checkValidationLayerSupport() )
		printf( "[ERROR] Validation layers requested, but not available!" );
	
	VkApplicationInfo app_info{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Vulkan Backend",
		.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
		.pEngineName = "none",
		.engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
		.apiVersion = VK_API_VERSION_1_3
	};

	std::vector<const char*> extensions = getRequiredExtensions();

	VkInstanceCreateInfo create_info{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo        = &app_info,
		.enabledExtensionCount   = (uint32_t)extensions.size(),
		.ppEnabledExtensionNames = extensions.data(),
	};

	
	if ( m_enable_validation_layers )
	{
		create_info.enabledLayerCount = (uint32_t)m_validation_layers.size();
		create_info.ppEnabledLayerNames = m_validation_layers.data();
	}
	else
		create_info.enabledLayerCount = 0;



	const VkResult result = vkCreateInstance( &create_info, nullptr, &m_instance );
	if( result == VK_SUCCESS )
		printf( "[INFO] Created Vulkan Instance.\n" );
	else
		printErrorResult( "[FATAL] Failed to create Vulkan Instance:", result);
}

void cm::cRenderer::setupDebugMessenger( void )
{
	if ( !m_enable_validation_layers )
		return;

	VkDebugUtilsMessengerCreateInfoEXT create_info{
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = debugCallback,
		.pUserData       = nullptr
	};

	const VkResult result = CreateDebugUtilsMessengerEXT( m_instance, &create_info, nullptr, &m_debug_messenger );
	if ( result == VK_SUCCESS )
		printf( "[INFO] Setup Debug Messenger.\n" );
	else
		printErrorResult( "[ERROR] Failed to setup Debug Messenger:", result );
}

///////////////////////////////////////////////////////////////////////////////////////

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

	int selected = -1;
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
		printf( "[INFO] Selected physical device %i.\n", selected );
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createLogicalDevice( void )
{
	sQueueFamilyIndices indices = findQueueFamilies( m_physical_device );
	float queue_priority = 1.0f;
	
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };

	for ( uint32_t queue_family : unique_queue_families )
	{
		VkDeviceQueueCreateInfo queue_create_info{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queue_family,
			.queueCount = 1,
			.pQueuePriorities = &queue_priority
		};

		queue_create_infos.push_back( queue_create_info );
	}

	/* create logical device */
	VkPhysicalDeviceFeatures device_features{};
	VkDeviceCreateInfo create_info{
		.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount    = (uint32_t)queue_create_infos.size(),
		.pQueueCreateInfos       = queue_create_infos.data(),
		.enabledLayerCount       = 0,
		.enabledExtensionCount   = (uint32_t)( m_device_extensions.size() ),
		.ppEnabledExtensionNames = m_device_extensions.data(),
		.pEnabledFeatures        = &device_features
	};

	const VkResult result = vkCreateDevice( m_physical_device, &create_info, nullptr, &m_device );
	if ( result == VK_SUCCESS )
		printf( "[INFO] Created Logical Device.\n" );
	else
		printErrorResult( "[FATAL] Failed to create Logical Device:", result );
	
	vkGetDeviceQueue( m_device, indices.graphics_family.value(), 0, &m_graphics_queue );
	vkGetDeviceQueue( m_device, indices.present_family.value(),  0, &m_present_queue );
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createSurface( void )
{
	m_active_window->createSurface( m_instance, &m_surface );
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createSwapChain( void )
{
	sSwapChainSupportDetails sc_support = querySwapChainSupport( m_physical_device );

	VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat( sc_support.formats );
	VkPresentModeKHR present_mode     = chooseSwapPresentMode  ( sc_support.present_modes );
	VkExtent2D extent                 = chooseSwapExtent       ( sc_support.capabilities );

	uint32_t num_images = sc_support.capabilities.minImageCount + 1;
	if ( sc_support.capabilities.maxImageCount > 0 && num_images > sc_support.capabilities.maxImageCount )
		num_images = sc_support.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR create_info{
		.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface          = m_surface,
		.minImageCount    = num_images,
		.imageFormat      = surface_format.format,
		.imageColorSpace  = surface_format.colorSpace,
		.imageExtent      = extent,
		.imageArrayLayers = 1,
		.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = sc_support.capabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = present_mode,
		.clipped = VK_TRUE,
		.oldSwapchain = VK_NULL_HANDLE // TODO: allow for recreation
	};

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

	const VkResult result = vkCreateSwapchainKHR( m_device, &create_info, nullptr, &m_swapchain );

	if ( result == VK_SUCCESS )
		printf("[INFO] Created Swapchain.\n");
	else
		printErrorResult( "[FATAL] Failed to create Swapchain:", result);

	/* get swapchain images */
	VECTOR_VK_GET( m_swapchain_images, vkGetSwapchainImagesKHR, m_device, m_swapchain );

	m_swapchain_image_format = surface_format.format;
	m_swapchain_extent       = extent;
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createImageViews( void )
{
	m_swapchain_image_views.resize( m_swapchain_images.size() );
	for ( size_t i = 0; i < m_swapchain_image_views.size(); i++ )
	{
		VkImageViewCreateInfo create_info{
			.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image    = m_swapchain_images[ i ],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format   = m_swapchain_image_format,
			.components = {
				VK_COMPONENT_SWIZZLE_IDENTITY, // r
				VK_COMPONENT_SWIZZLE_IDENTITY, // g
				VK_COMPONENT_SWIZZLE_IDENTITY, // b
				VK_COMPONENT_SWIZZLE_IDENTITY  // a
			},
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		const VkResult result = vkCreateImageView( m_device, &create_info, nullptr, &m_swapchain_image_views[ i ] );
		if ( result == VK_SUCCESS )
			printf( "[INFO] Created Swapchain ImageView %i.\n", (int)i );
		else
		{
			printf( "[FATAL] Failed to create Swapchain ImageView %i", (int)i );
			printErrorResult( ":", result );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createRenderPass( void )
{
	VkAttachmentDescription color_attachment{
		.format = m_swapchain_image_format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference color_attachment_ref{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass{
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_ref
	};
	
	VkSubpassDependency dependency{
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};

	VkRenderPassCreateInfo render_pass_info{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &color_attachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	const VkResult result = vkCreateRenderPass( m_device, &render_pass_info, nullptr, &m_render_pass );
	if ( result == VK_SUCCESS )
		printf( "[INFO] Created Render Pass\n" );
	else
		printErrorResult( "[FATAL] Failed to create Render Pass:", result );

}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createGraphicsPipeline( void )
{
	auto vert_shader_code = readFile( "vert.spv" );
	auto frag_shader_code = readFile( "frag.spv" );

	VkShaderModule vert_shader_module = createShaderModule( vert_shader_code );
	VkShaderModule frag_shader_module = createShaderModule( frag_shader_code );

	/* vertex shader */
	VkPipelineShaderStageCreateInfo vert_shader_stage_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = vert_shader_module,
		.pName = "main"
	};

	/* fragment shader */
	VkPipelineShaderStageCreateInfo frag_shader_stage_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = frag_shader_module,
		.pName = "main"
	};

	/* shader stage */
	VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

	/* dynamic state */
	std::vector<VkDynamicState> dynamic_states = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = (uint32_t)dynamic_states.size(),
		.pDynamicStates = dynamic_states.data()
	};

	/* vertex input */
	VkPipelineVertexInputStateCreateInfo vertex_input_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 0,
		.pVertexBindingDescriptions = nullptr,
		.vertexAttributeDescriptionCount = 0,
		.pVertexAttributeDescriptions = nullptr
	};

	/* input assembly */
	VkPipelineInputAssemblyStateCreateInfo input_assemply_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	/* viewport */
	/*
	VkViewport viewport{
		.x = 0.0f,
		.y = 0.0f,
		.width  = (float)m_swapchain_extent.width,
		.height = (float)m_swapchain_extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D scissor{
		.offset = { 0, 0 },
		.extent = m_swapchain_extent
	};
	*/

	/* viewport state */
	VkPipelineViewportStateCreateInfo viewport_state_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		//.pViewports = &viewport,
		.scissorCount = 1
		//.pScissors = &scissor
	};

	/* rasterizer */
	VkPipelineRasterizationStateCreateInfo rasterizer_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.lineWidth = 1.0f
	};

	/* multisampling */
	VkPipelineMultisampleStateCreateInfo multisample_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable   = VK_FALSE,
		.minSampleShading      = 1.0f,
		.pSampleMask           = nullptr,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable      = VK_FALSE
	};

	/* blend attachment */
	VkPipelineColorBlendAttachmentState blend_attachment_info{
		.blendEnable = VK_FALSE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,  // Optional
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
		.colorBlendOp        = VK_BLEND_OP_ADD,      // Optional
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,  // Optional
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
		.alphaBlendOp        = VK_BLEND_OP_ADD,      // Optional
		.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	/* blend state */
	VkPipelineColorBlendStateCreateInfo blend_state_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable   = VK_FALSE,
		.attachmentCount = 1,
		.pAttachments    = &blend_attachment_info
	};

	VkPipelineLayoutCreateInfo pipeline_layout_info{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = nullptr,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = nullptr
	};

	const VkResult pipeline_layout_result = vkCreatePipelineLayout( m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout );
	if ( pipeline_layout_result == VK_SUCCESS )
		printf( "[INFO] Created Pipeline Layout.\n" );
	else
		printErrorResult( "[FATAL] Failed to create Pipeline Layout:", pipeline_layout_result );


	/* pipeline object */
	VkGraphicsPipelineCreateInfo pipeline_info{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount          = 2,
		.pStages             = shader_stages,
		.pVertexInputState   = &vertex_input_info,
		.pInputAssemblyState = &input_assemply_info,
		.pViewportState      = &viewport_state_info,
		.pRasterizationState = &rasterizer_info,
		.pMultisampleState   = &multisample_info,
		.pDepthStencilState  = nullptr,
		.pColorBlendState    = &blend_state_info,
		.pDynamicState       = &dynamic_state_info,
		.layout              = m_pipeline_layout,
		.renderPass          = m_render_pass
	};


	const VkResult pipeline_result = vkCreateGraphicsPipelines( m_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_graphics_pipeline );
	if ( pipeline_result == VK_SUCCESS )
		printf( "[INFO] Created Graphics Pipeline.\n" );
	else
		printErrorResult( "[FATAL] Failed to create Graphics Pipeline:", pipeline_result );

	/* cleanup */
	vkDestroyShaderModule( m_device, vert_shader_module, nullptr );
	vkDestroyShaderModule( m_device, frag_shader_module, nullptr );

}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createFramebuffers( void )
{
	m_swapchain_framebuffers.resize( m_swapchain_images.size() );

	for ( int i = 0; i < m_swapchain_image_views.size(); i++ )
	{
		VkImageView attachments[] = {
			m_swapchain_image_views[ i ]
		};

		VkFramebufferCreateInfo framebuffer_info{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = m_render_pass,
			.attachmentCount = 1,
			.pAttachments = attachments,
			.width = m_swapchain_extent.width,
			.height = m_swapchain_extent.height,
			.layers = 1
		};



		const VkResult result = vkCreateFramebuffer( m_device, &framebuffer_info, nullptr, &m_swapchain_framebuffers[ i ] );
		if ( result == VK_SUCCESS )
			printf( "[INFO] Created Framebuffer %i.\n", i );
		else
		{
			printf( "[FATAL] Failed to create Framebuffer %i", (int)i );
			printErrorResult( ":", result );
		}

	}
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createCommandPool( void )
{
	sQueueFamilyIndices queue_family_indices = findQueueFamilies( m_physical_device );

	VkCommandPoolCreateInfo create_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = queue_family_indices.graphics_family.value()
	};

	const VkResult result = vkCreateCommandPool( m_device, &create_info, nullptr, &m_command_pool );
	if ( result == VK_SUCCESS )
		printf( "[INFO] Created Command Pool.\n" );
	else
		printErrorResult( "[FATAL] Failed to create Command Pool:", result );
	
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createCommandBuffers( void )
{
	m_command_buffers.resize( m_max_frames_in_flight ); 

	VkCommandBufferAllocateInfo alloc_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = m_command_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = (uint32_t)m_command_buffers.size()
	};

	const VkResult result = vkAllocateCommandBuffers( m_device, &alloc_info, m_command_buffers.data() );
	if ( result == VK_SUCCESS )
		printf( "[INFO] Created Command Buffers.\n" );
	else
		printErrorResult( "[FATAL] Failed to create Command Buffers:", result );
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::createSyncObjects( void )
{
	m_image_available_semaphores.resize( m_max_frames_in_flight );
	m_render_finished_semaphores.resize( m_max_frames_in_flight );
	m_in_flight_fences.resize          ( m_max_frames_in_flight );

	VkSemaphoreCreateInfo semaphore_info{
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	VkFenceCreateInfo fence_info{
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	for ( int i = 0; i < m_max_frames_in_flight; i++ )
	{
		const VkResult result0 = vkCreateSemaphore( m_device, &semaphore_info, nullptr, &m_image_available_semaphores[ i ] );
		const VkResult result1 = vkCreateSemaphore( m_device, &semaphore_info, nullptr, &m_render_finished_semaphores[ i ] );
		const VkResult result2 = vkCreateFence    ( m_device, &fence_info,     nullptr, &m_in_flight_fences[ i ] );

		if ( result0 != VK_SUCCESS || result1 != VK_SUCCESS || result2 != VK_SUCCESS )
		{
			printf( "[FATAL] Failed to create semaphores for frame %i!\n", i );
			printErrorResult( "{m_image_available_semaphore}:", result0 );
			printErrorResult( "{m_render_finished_semaphore}:", result1 );
			printErrorResult( "{m_in_flight_fence}:", result2 );
		}
	}
}

void cm::cRenderer::cleanupSwapchain()
{
	for ( auto framebuffer : m_swapchain_framebuffers )
		vkDestroyFramebuffer( m_device, framebuffer, nullptr );

	for ( auto image_view : m_swapchain_image_views )
		vkDestroyImageView( m_device, image_view, nullptr );

	vkDestroySwapchainKHR( m_device, m_swapchain, nullptr );
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::recordCommandBuffer( VkCommandBuffer _command_buffer, uint32_t _image_index )
{
	/* begin */
	VkCommandBufferBeginInfo begin_info{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	};

	const VkResult result = vkBeginCommandBuffer( _command_buffer, &begin_info );
	if ( result != VK_SUCCESS )
		printErrorResult( "[FATAL] Failed to Begin Command Buffer:", result );
	
	VkClearValue clear_color = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
	
	VkRenderPassBeginInfo render_pass_info{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass = m_render_pass,
		.framebuffer = m_swapchain_framebuffers[ _image_index ],
		.renderArea = {
			.offset = { 0, 0 },
			.extent = m_swapchain_extent
		},
		.clearValueCount = 1,
		.pClearValues = &clear_color
	};

	vkCmdBeginRenderPass( _command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE );

	vkCmdBindPipeline( _command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline );

	/* setup dynamic viewport and scissor */
	VkViewport viewport{
		.x        = 0.0f,
		.y        = 0.0f,
		.width    = (float)m_swapchain_extent.width,
		.height   = (float)m_swapchain_extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	vkCmdSetViewport( _command_buffer, 0, 1, &viewport );


	VkRect2D scissor{
		.offset = { 0, 0 },
		.extent = m_swapchain_extent
	};

	vkCmdSetScissor( _command_buffer, 0, 1, &scissor );

	vkCmdDraw( _command_buffer, 3, 1, 0, 0 );

	vkCmdEndRenderPass( _command_buffer );
	
	if ( vkEndCommandBuffer( _command_buffer ) != VK_SUCCESS )
		printErrorResult( "[FATAL] Failed to End Command Buffer:", result );
}

///////////////////////////////////////////////////////////////////////////////////////

void cm::cRenderer::printErrorResult( const std::string& _message, VkResult _result )
{
	printf( "%s\n   ", _message.c_str() );

	switch ( _result )
	{
	case VK_SUCCESS:                         printf( "SUCCESS" );                   break;
	case VK_ERROR_VALIDATION_FAILED_EXT:     printf( "VALIDATION FAILED" );         break;
	case VK_ERROR_OUT_OF_HOST_MEMORY:        printf( "OUT OF HOST MEMORY" );        break;
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:      printf( "OUT OF DEVICE MEMORY" );      break;
	case VK_ERROR_INITIALIZATION_FAILED:     printf( "INITIALIZATION FAILED" );     break;
	case VK_ERROR_EXTENSION_NOT_PRESENT:     printf( "EXTENSION NOT PRESENT" );     break;
	case VK_ERROR_FEATURE_NOT_PRESENT:       printf( "FEATURE NOT PRESENT" );       break;
	case VK_ERROR_TOO_MANY_OBJECTS:          printf( "TOO MANY OBJECTS" );          break;
	case VK_ERROR_DEVICE_LOST:               printf( "DEVICE LOST" );               break;
	case VK_ERROR_LAYER_NOT_PRESENT:         printf( "LAYER NOT PRESENT" );         break;
	case VK_ERROR_INCOMPATIBLE_DRIVER:       printf( "INCOMPATIBLE DRIVER" );       break;
	case VK_ERROR_SURFACE_LOST_KHR:          printf( "SURFACE LOST" );              break;
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:  printf( "NATIVE WINDOW IN USE" );      break;
	case VK_ERROR_COMPRESSION_EXHAUSTED_EXT: printf( "COMPRESSION EXHAUSTED" );     break;
	case VK_ERROR_INVALID_SHADER_NV:         printf( "INVALID SHADER NV" );         break;
	case VK_PIPELINE_COMPILE_REQUIRED_EXT:   printf( "PIPELINE COMPILE REQUIRED" ); break;
	}

	printf( "\n" );
}

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

bool cm::cRenderer::checkValidationLayerSupport( void )
{
	uint32_t num_layers;
	vkEnumerateInstanceLayerProperties( &num_layers, nullptr );

	std::vector<VkLayerProperties> available_layers( num_layers );
	vkEnumerateInstanceLayerProperties( &num_layers, available_layers.data() );

	for ( const char* layer_name : m_validation_layers )
	{
		bool layer_found = false;

		for ( const auto& layerProperties : available_layers )
		{
			if ( strcmp( layer_name, layerProperties.layerName ) == 0 )
			{
				layer_found = true;
				break;
			}
		}

		if ( !layer_found )
		{
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////

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

std::vector<const char*> cm::cRenderer::getRequiredExtensions()
{
	uint32_t num_glfw_extension = 0;
	const char** glfw_extensions;
	glfw_extensions = m_active_window->getRequiredInstanceExtensions( &num_glfw_extension );

	std::vector<const char*> extensions( glfw_extensions, glfw_extensions + num_glfw_extension );

	if ( m_enable_validation_layers )
		extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );

	return extensions;
}

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

VkSurfaceFormatKHR cm::cRenderer::chooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& _available_formats )
{
	for ( const auto& available_format : _available_formats )
		if ( available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR )
			return available_format;

	return _available_formats[ 0 ];
}

///////////////////////////////////////////////////////////////////////////////////////

VkPresentModeKHR cm::cRenderer::chooseSwapPresentMode( const std::vector<VkPresentModeKHR>& _available_present_modes )
{
	for ( const auto& available_present_mode : _available_present_modes )
		if ( available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR )
			return available_present_mode;
	
	return VK_PRESENT_MODE_FIFO_KHR;
}

///////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////

VkShaderModule cm::cRenderer::createShaderModule( const std::vector<char>& _code )
{
	VkShaderModuleCreateInfo create_info{};

	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = _code.size();
	create_info.pCode = reinterpret_cast<const uint32_t*>( _code.data() );

	VkShaderModule shader_module;
	const VkResult result = vkCreateShaderModule( m_device, &create_info, nullptr, &shader_module );
	if ( result == VK_SUCCESS )
		printf( "[INFO] Created Shader Module.\n" );
	else
		printErrorResult( "[ERROR] Failed to create Shader Module:", result );
	
	return shader_module;
}

///////////////////////////////////////////////////////////////////////////////////////
