#pragma once

#include <vulkan/vulkan.h>

#include <unordered_map>
#include <vector>

namespace wv {

class CommandBuffer
{
	friend class Renderer;

public:
	CommandBuffer( VkDevice _device, VkCommandPool _pool );

	VkCommandBuffer getUnderlying() const { return m_cmd; }

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

	void pushConstant( VkPipelineLayout _pipelineLayout, VkShaderStageFlags _stage, uint32_t _offset, uint32_t _size, const void* _data );
	
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

///////////////////////////////////////////////////////////////////////////////////////

class FenceRing
{
public:
	void initialize( VkDevice _device, uint32_t _cycleSize = 2 )
	{
		m_device = _device;
		m_cycleSize = _cycleSize;
		m_fences.resize( _cycleSize );

		VkFenceCreateInfo fenceCreateInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for ( uint32_t i = 0; i < _cycleSize; i++ )
			vkCreateFence( _device, &fenceCreateInfo, nullptr, &m_fences[ i ] );
	}

	void shutdown() {
		for ( uint32_t i = 0; i < m_cycleSize; i++ )
			vkDestroyFence( m_device, m_fences[ i ], nullptr );
	}

	// waits and resets fence
	void setCycle( uint32_t _cycle ) {
		m_cycleIndex = _cycle % m_cycleSize;

		vkWaitForFences( m_device, 1, &m_fences[ m_cycleIndex ], true, 1000000000 );
		vkResetFences( m_device, 1, &m_fences[ m_cycleIndex ] );
	}

	VkFence getFence() const { return m_fences[ m_cycleIndex ]; }

private:
	VkDevice m_device{ VK_NULL_HANDLE };

	uint32_t m_cycleSize{ 0 };
	uint32_t m_cycleIndex{ 0 };

	std::vector<VkFence> m_fences{};
};

///////////////////////////////////////////////////////////////////////////////////////

class SemaphoreRing
{
public:
	void initialize( VkDevice _device, uint32_t _cycleSize = 2 )
	{
		m_device = _device;
		m_cycleSize = _cycleSize;
		m_semaphores.resize( _cycleSize );

		VkSemaphoreCreateInfo semaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
		for ( uint32_t i = 0; i < _cycleSize; i++ )
			vkCreateSemaphore( m_device, &semaphoreCreateInfo, nullptr, &m_semaphores[ i ] );
	}

	void shutdown() {
		for ( uint32_t i = 0; i < m_cycleSize; i++ )
			vkDestroySemaphore( m_device, m_semaphores[ i ], nullptr );
	}

	// waits and resets fence
	void setCycle( uint32_t _cycle ) {
		m_cycleIndex = _cycle % m_cycleSize;
	}

	VkSemaphore getSemaphore() const { return m_semaphores[ m_cycleIndex ]; }

private:
	VkDevice m_device{ VK_NULL_HANDLE };

	uint32_t m_cycleSize{ 0 };
	uint32_t m_cycleIndex{ 0 };

	std::vector<VkSemaphore> m_semaphores{};
};

}