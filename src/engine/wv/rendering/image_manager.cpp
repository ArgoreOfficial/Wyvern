#include "image_manager.h"

#include <wv/rendering/renderer.h>

///////////////////////////////////////////////////////////////////////////////////////

wv::ImageID wv::ImageManager::createImage( VkFormat _format, VkExtent3D _extent, VkImageUsageFlags _usage, bool _mipmapped )
{
	AllocatedImage allocatedImage{};
	allocatedImage.imageFormat = _format;
	allocatedImage.imageExtent = _extent;

	VkImageCreateInfo imageInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.format = allocatedImage.imageFormat;
	imageInfo.extent = _extent;
	imageInfo.mipLevels   = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.tiling  = VK_IMAGE_TILING_OPTIMAL;
	imageInfo.usage   = _usage;

	if ( _mipmapped )
		imageInfo.mipLevels = static_cast<uint32_t>( std::floor( std::log2( std::max( _extent.width, _extent.height ) ) ) ) + 1;

	VmaAllocationCreateInfo allocInfo{};
	allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	if ( _format == VK_FORMAT_D32_SFLOAT )
		aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;

	vmaCreateImage( m_renderer->m_allocator, &imageInfo, &allocInfo, &allocatedImage.image, &allocatedImage.allocation, nullptr );

	VkImageViewCreateInfo viewInfo{ .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.image    = allocatedImage.image;
	viewInfo.format   = allocatedImage.imageFormat;

	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.subresourceRange.aspectMask = aspectFlag;

	viewInfo.subresourceRange.levelCount = imageInfo.mipLevels;

	vkCreateImageView( m_renderer->m_device, &viewInfo, nullptr, &allocatedImage.imageView );

	return m_allocatedImages.emplace( allocatedImage );
}

wv::ImageID wv::ImageManager::createImage( void* _data, VkFormat _format, VkExtent3D _extent, VkImageUsageFlags _usage, bool _mipmapped )
{
	size_t dataSize = _extent.width * _extent.height * _extent.depth * 4;

	AllocatedBuffer uploadBuffer = m_renderer->createBuffer(
		dataSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_MEMORY_USAGE_CPU_TO_GPU
	);

	memcpy( uploadBuffer.info.pMappedData, _data, dataSize );

	ImageID imageID = createImage(
		_format,
		_extent,
		_usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		_mipmapped
	);

	if ( !imageID.isValid() )
		return imageID;

	AllocatedImage& allocatedImage = m_allocatedImages.at( imageID );

	m_renderer->immediateCmdSubmit( [ & ]( CommandBuffer& _cmd ) {
		_cmd.transitionImage( allocatedImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );

		VkBufferImageCopy copyRegion{};

		copyRegion.bufferOffset = 0;
		copyRegion.bufferRowLength = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel = 0;
		copyRegion.imageSubresource.baseArrayLayer = 0;
		copyRegion.imageSubresource.layerCount = 1;
		copyRegion.imageExtent = _extent;

		vkCmdCopyBufferToImage( _cmd.getUnderlying(), uploadBuffer.buffer, allocatedImage.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		_cmd.transitionImage( allocatedImage.image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	} );

	m_renderer->destroyBuffer( uploadBuffer );

	return imageID;
}

void wv::ImageManager::destroyImage( ImageID _image )
{
	if ( !_image.isValid() )
		return;

	AllocatedImage image = m_allocatedImages.at( _image );
	m_allocatedImages.erase( _image );

	vkDestroyImageView( m_renderer->m_device, image.imageView, nullptr );
	vmaDestroyImage( m_renderer->m_allocator, image.image, image.allocation );
}