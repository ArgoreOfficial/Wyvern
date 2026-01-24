#pragma once

#include <wv/resource_id.h>
#include <wv/helpers/unordered_array.hpp>

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <unordered_map>

namespace wv {

using ImageID = TResourceID<uint32_t, struct ImageID_t>;

struct AllocatedImage
{
	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	VkExtent3D imageExtent;
	VkFormat imageFormat;
};

class Renderer;

class ImageManager
{
public:
	ImageManager( Renderer* _renderer ) : m_renderer{ _renderer } { };

	ImageID createImage( VkFormat _format, VkExtent3D _extent, VkImageUsageFlags _usage, bool _mipmapped = false );
	ImageID createImage( void* _data, VkFormat _format, VkExtent3D _extent, VkImageUsageFlags _usage, bool _mipmapped = false );
	void destroyImage( ImageID _image );

	AllocatedImage getAllocatedImage( ImageID _imageID ) const {
		if ( m_allocatedImages.contains( _imageID ) )
			return m_allocatedImages.at( _imageID );
		return {};
	}

private:
	Renderer* m_renderer = nullptr;

	unordered_array<ImageID, AllocatedImage> m_allocatedImages;

};

}