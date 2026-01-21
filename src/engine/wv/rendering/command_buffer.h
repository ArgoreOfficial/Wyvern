#pragma once

#include <vulkan/vulkan.h>

#include <unordered_map>

namespace wv {

class CommandBuffer
{
	friend class Renderer;

public:
	CommandBuffer( VkDevice _device, VkCommandPool _pool );

	void begin() const;
	void end() const;
	void reset();

	void submit( VkQueue _queue, VkSemaphoreSubmitInfo* _waitInfo, VkSemaphoreSubmitInfo* _signalInfo, VkFence _fence );

	void beginRendering( float _width, float _height, VkImageView _colorView, VkImageView _depthView );
	void endRendering();

	// State

	void setViewport( float _x, float _y, float _width, float _height, float _minDepth = 0.0f, float _maxDepth = 1.0f );
	void setScissor( float _x, float _y, float _width, float _height );
	void setDepthTest( bool _test, bool _write, VkCompareOp _compareOp );
	void setStencilTest( bool _enabled );

	void bindIndexBuffer( VkBuffer _buffer, VkDeviceSize _offset, VkIndexType _type );

	void bindPipeline( VkPipelineBindPoint _bindPoint, VkPipeline _pipeline );
	void bindDescriptorSets( VkPipelineBindPoint _bindPoint, VkPipelineLayout _layout, uint32_t _firstSet, uint32_t _descriptorSetCount, VkDescriptorSet* _descriptorSets );

	void pushConstant( VkPipelineLayout _pipelineLayout, VkShaderStageFlags _stage, uint32_t _offset, uint32_t _size, void* _data );
	
	// Image

	void transitionImage( VkImage _image, VkImageLayout _currentLayout, VkImageLayout _newLayout );
	void transitionImage( VkImage _image, VkImageLayout _newLayout ) {
		if ( m_imageLastKnownLayout.contains( _image ) )
			transitionImage( _image, m_imageLastKnownLayout.at( _image ), _newLayout);
		else
			transitionImage( _image, VK_IMAGE_LAYOUT_UNDEFINED, _newLayout );
	}

	void copyImageToImage( VkImage _source, VkImage _destination, VkExtent2D _srcSize, VkExtent2D _dstSize );
	void clearColorImage( VkImage _image, VkImageLayout _layout, VkClearColorValue* _clearValue );

	// Draw/Dispatch

	void dispatch( uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ );
	void draw( uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, int32_t _vertexOffset, uint32_t _firstInstance );

protected:

	std::unordered_map<VkImage, VkImageLayout> m_imageLastKnownLayout;

	VkCommandBuffer m_cmd{};
	
};

}