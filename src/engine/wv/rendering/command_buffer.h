#pragma once

#include <vulkan/vulkan.h>

#include <unordered_map>
#include <vector>

namespace wv {

static VkViewport makeVkViewport( float _x, float _y, float _width, float _height, float _minDepth = 0.0f, float _maxDepth = 1.0f ) {
	VkViewport viewport{};
	viewport.x = _x;
	viewport.y = _y;
	viewport.width = _width;
	viewport.height = _height;
	viewport.minDepth = _minDepth;
	viewport.maxDepth = _maxDepth;
	return viewport;
}

static VkRect2D makeVkRect2D( float _x, float _y, float _width, float _height ) {
	VkRect2D scissor{};
	scissor.offset.x = _x;
	scissor.offset.y = _y;
	scissor.extent.width = _width;
	scissor.extent.height = _height;
	return scissor;
}

static VkSubmitInfo2 makeVkSubmitInfo2(
	uint32_t _numCmd,    VkCommandBufferSubmitInfo* _cmds, 
	uint32_t _numSignal, VkSemaphoreSubmitInfo* _signals, 
	uint32_t _numWait,   VkSemaphoreSubmitInfo* _waits )
{
	VkSubmitInfo2 info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
	info.waitSemaphoreInfoCount = _numWait;
	info.pWaitSemaphoreInfos = _waits;

	info.signalSemaphoreInfoCount = _numSignal;
	info.pSignalSemaphoreInfos = _signals;

	info.commandBufferInfoCount = _numCmd;
	info.pCommandBufferInfos = _cmds;

	return info;
}

static VkImageSubresourceRange makeVkImageSubresourceRange( VkImageAspectFlags _aspectMask )
{
	VkImageSubresourceRange subImage{};
	subImage.aspectMask = _aspectMask;
	subImage.baseMipLevel = 0;
	subImage.levelCount = VK_REMAINING_MIP_LEVELS;
	subImage.baseArrayLayer = 0;
	subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	return subImage;
}

static VkImageSubresourceLayers makeVkImageSubresourceLayers( VkImageAspectFlags _aspectMask, uint32_t _mipLevel, uint32_t _baseArrayLayer, uint32_t _layerCount )
{
	VkImageSubresourceLayers layers{};
	layers.aspectMask     = _aspectMask;
	layers.mipLevel       = _mipLevel;
	layers.baseArrayLayer = _baseArrayLayer;
	layers.layerCount     = _layerCount;
	return layers;
}

///////////////////////////////////////////////////////////////////////////////////////

class CommandBuffer
{
	friend class Renderer;

public:
	CommandBuffer( VkDevice _device, VkCommandPool _pool );

	VkCommandBuffer getUnderlying() const { return m_cmd; }

	void submit( VkQueue _queue, VkSemaphoreSubmitInfo* _waitInfo, VkSemaphoreSubmitInfo* _signalInfo, VkFence _fence );

	void beginRendering( float _width, float _height, VkImageView _colorView, VkImageView _depthView );
	
	// Image

	void transitionImage( VkImage _image, VkImageLayout _oldLayout, VkImageLayout _newLayout );
	
	void copyImageToImage( VkImage _src, VkImage _dst, VkExtent2D _srcSize, VkExtent2D _dstSize );
	
protected:

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