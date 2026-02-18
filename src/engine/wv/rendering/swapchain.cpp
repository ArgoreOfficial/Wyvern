#include "swapchain.h"

#include <wv/debug/error.h>
#include <wv/debug/log.h>

#include <wv/math/math.h>

void wv::Swapchain::createSwapchain( uint32_t _width, uint32_t _height )
{
	m_supportDetails = querySwapchainSupport();

	m_format = chooseSurfaceFormat( VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR );
	VkPresentModeKHR   presentMode   = chooseSwapPresentMode( VK_PRESENT_MODE_FIFO_KHR );
	m_swapchainExtent = chooseSwapExtent( _width, _height );

	uint32_t imageCount = m_supportDetails.capabilities.minImageCount + 1;
	
	if( m_supportDetails.capabilities.maxImageCount > 0 )
		imageCount = wv::Math::clamp( imageCount, imageCount, m_supportDetails.capabilities.maxImageCount );

	VkSwapchainCreateInfoKHR createInfo{ .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	createInfo.surface = m_surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = m_format.format;
	createInfo.imageColorSpace = m_format.colorSpace;
	createInfo.imageExtent = m_swapchainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	// should check graphcis and present queue but I'm lazy
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		// createInfo.queueFamilyIndexCount = 0;
		// createInfo.pQueueFamilyIndices = nullptr;
	}

	createInfo.preTransform = m_supportDetails.capabilities.currentTransform;

	// NOTE: alpha will still be stored in the swapchain image 
	// this means OBS game capture will show up transparent even 
	// if the window appears not to
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	// Create swapchain object
	VkResult res = vkCreateSwapchainKHR( m_device, &createInfo, nullptr, &m_swapchain );
	WV_ASSERT( res == VK_SUCCESS );

	// Get swapchain images

	m_swapchainImages     = getSwapchainImages();
	m_swapchainImageViews = createSwapchainImageViews( m_swapchainImages );

	// Create swapchain semaphores

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

	VkResult res = vkQueuePresentKHR( _queue, &presentInfo );
	return res;
}

wv::SwapchainSupportDetails wv::Swapchain::querySwapchainSupport() const
{
	SwapchainSupportDetails details{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( m_physicalDevice, m_surface, &details.capabilities );

	// Get formats
	{
		uint32_t numFormats;
		vkGetPhysicalDeviceSurfaceFormatsKHR( m_physicalDevice, m_surface, &numFormats, nullptr );

		WV_ASSERT( numFormats != 0 );

		details.formats.resize( numFormats );
		vkGetPhysicalDeviceSurfaceFormatsKHR( m_physicalDevice, m_surface, &numFormats, details.formats.data() );
	}

	// Get present modes
	{
		uint32_t numPresentModes;
		vkGetPhysicalDeviceSurfacePresentModesKHR( m_physicalDevice, m_surface, &numPresentModes, nullptr );

		WV_ASSERT( numPresentModes != 0 );

		details.presentModes.reserve( numPresentModes );
		vkGetPhysicalDeviceSurfacePresentModesKHR( m_physicalDevice, m_surface, &numPresentModes, details.presentModes.data() );
	}

	return details;
}

VkSurfaceFormatKHR wv::Swapchain::chooseSurfaceFormat( VkFormat _desiredFormat, VkColorSpaceKHR _desiredColorSpace ) const
{
	for ( const auto& availableFormat : m_supportDetails.formats )
		if ( availableFormat.format == _desiredFormat && availableFormat.colorSpace == _desiredColorSpace )
			return availableFormat;
	
	// better way of defaulting?

	return m_supportDetails.formats[ 0 ];
}

VkPresentModeKHR wv::Swapchain::chooseSwapPresentMode( VkPresentModeKHR _desiredPresentMode ) const
{
	for ( const auto& availablePresentMode : m_supportDetails.presentModes )
		if ( availablePresentMode == _desiredPresentMode )
			return availablePresentMode;
	
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D wv::Swapchain::chooseSwapExtent( uint32_t _desiredWidth, uint32_t _desiredHeight ) const
{
	if ( m_supportDetails.capabilities.currentExtent.width != UINT32_MAX )
		return m_supportDetails.capabilities.currentExtent; // already has extent, skip
	
	VkExtent2D minExtent = m_supportDetails.capabilities.minImageExtent;
	VkExtent2D maxExtent = m_supportDetails.capabilities.maxImageExtent;

	VkExtent2D extent{ 
		wv::Math::clamp( _desiredWidth,  minExtent.width, maxExtent.width ),
		wv::Math::clamp( _desiredHeight, minExtent.width, maxExtent.width )
	};

	return extent;
	
}

std::vector<VkImage> wv::Swapchain::getSwapchainImages() const
{
	uint32_t swapchainImageCount{ 0 };
	std::vector<VkImage> swapchainImages{};

	vkGetSwapchainImagesKHR( m_device, m_swapchain, &swapchainImageCount, nullptr );
	
	if ( swapchainImageCount > 0 )
	{
		swapchainImages.resize( swapchainImageCount );
		vkGetSwapchainImagesKHR( m_device, m_swapchain, &swapchainImageCount, swapchainImages.data() );
	}

	return swapchainImages;
}

std::vector<VkImageView> wv::Swapchain::createSwapchainImageViews( const std::vector<VkImage>& _images ) const
{
	std::vector<VkImageView> imageViews{};
	imageViews.resize( _images.size() );

	for ( size_t i = 0; i < _images.size(); i++ )
	{
		VkImageViewCreateInfo createInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };

		createInfo.image = _images[ i ];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_format.format;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		WV_ASSERT( vkCreateImageView( m_device, &createInfo, nullptr, &imageViews[ i ] ) == VK_SUCCESS );
	}

	return imageViews;
}
