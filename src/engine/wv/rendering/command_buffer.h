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

	void bindPipeline( VkPipelineBindPoint _bindPoint, VkPipeline _pipeline ) {
		vkCmdBindPipeline( m_cmd, _bindPoint, _pipeline );
	}

	void bindDescriptorSets( VkPipelineBindPoint _bindPoint, VkPipelineLayout _layout, uint32_t _firstSet, uint32_t _descriptorSetCount, VkDescriptorSet* _descriptorSets ) {
		vkCmdBindDescriptorSets( m_cmd, _bindPoint, _layout, _firstSet, _descriptorSetCount, _descriptorSets, 0, nullptr );
	}

	void dispatch( uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ ) {
		vkCmdDispatch( m_cmd, _groupCountX, _groupCountY, _groupCountZ );
	}

protected:

	VkCommandBuffer m_cmd{};
	
};

}