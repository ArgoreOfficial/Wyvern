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

#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////

bool wv::Renderer::initialize()
{
	if ( !initVulkan() ) return false;
	if ( !initSwapchain() ) return false;
	if ( !initCommands() ) return false;
	if ( !initSyncStructures() ) return false;

	return true;
}

void wv::Renderer::shutdown()
{
}

void wv::Renderer::prepare( uint32_t _width, uint32_t _height )
{
}

void wv::Renderer::finalize()
{
}

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

	return true;
}

bool wv::Renderer::initSwapchain()
{
	return true;
}

bool wv::Renderer::initCommands()
{
	return true;
}

bool wv::Renderer::initSyncStructures()
{
	return true;
}
