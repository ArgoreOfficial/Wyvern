#pragma once

#include <vulkan/vulkan.h>

namespace wv {

class CommandBuffer
{
	friend class Renderer;

public:
	CommandBuffer( VkDevice _device, VkCommandPool _pool );

	void reset() { 
		vkResetCommandBuffer( m_cmd, 0 ); 
	}

	void begin() {
		VkCommandBufferBeginInfo cmdBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer( m_cmd, &cmdBeginInfo );
	}

	void end() {
		vkEndCommandBuffer( m_cmd );
	}

	void submit( VkQueue _queue, VkSemaphoreSubmitInfo* _waitInfo, VkSemaphoreSubmitInfo* _signalInfo, VkFence _fence );

	void transitionImage( VkImage _image, VkImageLayout _currentLayout, VkImageLayout _newLayout );
	void copyImageToImage( VkImage _source, VkImage _destination, VkExtent2D _srcSize, VkExtent2D _dstSize );
	void clearColorImage( VkImage _image, VkImageLayout _layout, VkClearColorValue* _clearValue );

protected:

	VkCommandBuffer m_cmd{};
	
};

}