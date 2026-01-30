#include "swapchain.h"

#include <auxiliary/vk-bootstrap/VkBootstrap.h>

#include <wv/math/math.h>

void wv::Swapchain::createSwapchain( uint32_t _width, uint32_t _height )
{ 
	vkb::SwapchainBuilder builder{ m_physicalDevice, m_device, m_surface };

	// must be at least 1
	_width  = wv::Math::max( _width, 1U );
	_height = wv::Math::max( _height, 1U );

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

void wv::Swapchain::destroySwapchain()
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

VkResult wv::Swapchain::acquireNextImage( VkSemaphore _acquireSemaphore, uint32_t* _outIndex ) const
{
	return vkAcquireNextImageKHR( 
		m_device, 
		m_swapchain, 
		1000000000, _acquireSemaphore, 
		nullptr, 
		_outIndex );
}

VkResult wv::Swapchain::present( uint32_t _imageIndex, VkQueue _queue ) const
{
	VkPresentInfoKHR presentInfo{ .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
	presentInfo.pSwapchains = &m_swapchain;
	presentInfo.swapchainCount = 1;
	presentInfo.pWaitSemaphores = &m_submitSemaphores[ _imageIndex ];
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &_imageIndex;

	return vkQueuePresentKHR( _queue, &presentInfo );
}
