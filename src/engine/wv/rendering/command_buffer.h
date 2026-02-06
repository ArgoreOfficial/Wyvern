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

void submit( VkCommandBuffer _cmd, VkQueue _queue, VkSemaphoreSubmitInfo* _waitInfo, VkSemaphoreSubmitInfo* _signalInfo, VkFence _fence );
void beginRendering( VkCommandBuffer _cmd, float _width, float _height, VkImageView _colorView, VkImageView _depthView );
void transitionImage( VkCommandBuffer _cmd, VkImage _image, VkImageLayout _oldLayout, VkImageLayout _newLayout );
void copyImageToImage( VkCommandBuffer _cmd, VkImage _src, VkImage _dst, VkExtent2D _srcSize, VkExtent2D _dstSize );

///////////////////////////////////////////////////////////////////////////////////////
	
class CommandPoolRing
{
public:
	void initialize( VkDevice _device, uint32_t _queueFamily, VkCommandPoolCreateFlags _flags, uint32_t _cycleSize = 2 )
	{
		m_device = _device;
		m_cycleSize = _cycleSize;

		VkCommandPoolCreateInfo commandPoolInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
		commandPoolInfo.flags = _flags;
		commandPoolInfo.queueFamilyIndex = _queueFamily;

		m_pools.resize( m_cycleSize );
		m_cmdVecs.resize( m_cycleSize );
		for ( uint32_t i = 0; i < m_cycleSize; i++ )
			vkCreateCommandPool( m_device, &commandPoolInfo, nullptr, &m_pools[ i ] );
		
	}

	void shutdown() 
	{
		for ( uint32_t i = 0; i < m_cycleSize; i++ )
		{
			vkDestroyCommandPool( m_device, m_pools[ i ], nullptr );
			m_cmdVecs[ i ].clear();
		}
	}

	// waits and resets fence
	void setCycle( uint32_t _cycle ) {
		m_cycleIndex = _cycle % m_cycleSize;

		clearPool( m_cycleIndex );
	}

	VkCommandBuffer createBuffer( VkCommandBufferLevel _level, bool _begin, VkCommandBufferUsageFlags _usageFlags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT ) 
	{
		VkCommandBufferAllocateInfo cmdAllocInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		cmdAllocInfo.commandPool = getPool();
		cmdAllocInfo.commandBufferCount = 1;
		cmdAllocInfo.level = _level; 

		VkCommandBuffer cmd{ VK_NULL_HANDLE };
		vkAllocateCommandBuffers( m_device, &cmdAllocInfo, &cmd );
		getCmdVec().push_back( cmd );
		
		if ( _begin )
		{
			VkCommandBufferBeginInfo cmdBeginInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
			cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			
			vkBeginCommandBuffer( cmd, &cmdBeginInfo );
		}

		return cmd;
	}

private:
	VkCommandPool                 getPool()   { return m_pools[ m_cycleIndex ]; }
	std::vector<VkCommandBuffer>& getCmdVec() { return m_cmdVecs[ m_cycleIndex ]; }

	void clearPool( uint32_t _cycle ) {
		if ( m_cmdVecs[ _cycle ].empty() )
			return;

		vkFreeCommandBuffers( 
			m_device, 
			m_pools[ _cycle ], 
			m_cmdVecs[ _cycle ].size(),
			m_cmdVecs[ _cycle ].data()
		);

		m_cmdVecs[ _cycle ].clear();
	}

	VkDevice m_device{ VK_NULL_HANDLE };

	uint32_t m_cycleSize{ 0 };
	uint32_t m_cycleIndex{ 0 };

	std::vector<VkCommandPool> m_pools{};
	std::vector<std::vector<VkCommandBuffer>> m_cmdVecs{};
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