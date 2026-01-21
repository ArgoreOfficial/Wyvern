#include "command_buffer.h"

///////////////////////////////////////////////////////////////////////////////////////

VkSubmitInfo2 submitInfo2( VkCommandBufferSubmitInfo* _cmd, VkSemaphoreSubmitInfo* _signalSemaphoreInfo, VkSemaphoreSubmitInfo* _waitSemaphoreInfo )
{
	VkSubmitInfo2 info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2 };
	info.waitSemaphoreInfoCount = _waitSemaphoreInfo == nullptr ? 0 : 1;
	info.pWaitSemaphoreInfos = _waitSemaphoreInfo;

	info.signalSemaphoreInfoCount = _signalSemaphoreInfo == nullptr ? 0 : 1;
	info.pSignalSemaphoreInfos = _signalSemaphoreInfo;

	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos = _cmd;

	return info;
}

///////////////////////////////////////////////////////////////////////////////////////

VkImageSubresourceRange imageSubresourceRange( VkImageAspectFlags _aspectMask )
{
	VkImageSubresourceRange subImage{};
	subImage.aspectMask = _aspectMask;
	subImage.baseMipLevel = 0;
	subImage.levelCount = VK_REMAINING_MIP_LEVELS;
	subImage.baseArrayLayer = 0;
	subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	return subImage;
}

///////////////////////////////////////////////////////////////////////////////////////

wv::CommandBuffer::CommandBuffer( VkDevice _device, VkCommandPool _pool )
{ 
	VkCommandBufferAllocateInfo cmdAllocInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	cmdAllocInfo.commandPool = _pool;
	cmdAllocInfo.commandBufferCount = 1;
	cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	vkAllocateCommandBuffers( _device, &cmdAllocInfo, &m_cmd );
}

void wv::CommandBuffer::begin() const
{
	VkCommandBufferBeginInfo cmdBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer( m_cmd, &cmdBeginInfo );
}

void wv::CommandBuffer::end() const
{
	vkEndCommandBuffer( m_cmd );
}

void wv::CommandBuffer::reset()
{
	vkResetCommandBuffer( m_cmd, 0 );
	m_imageLastKnownLayout.clear();
}

void wv::CommandBuffer::submit( VkQueue _queue, VkSemaphoreSubmitInfo* _waitInfo, VkSemaphoreSubmitInfo* _signalInfo, VkFence _fence )
{

	VkCommandBufferSubmitInfo cmdInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
	cmdInfo.commandBuffer = m_cmd;

	VkSubmitInfo2 submitInfo = submitInfo2( &cmdInfo, _signalInfo, _waitInfo );
	vkQueueSubmit2( _queue, 1, &submitInfo, _fence );
}

void wv::CommandBuffer::beginRendering( float _width, float _height, VkImageView _colorView, VkImageView _depthView )
{ 
	VkRenderingAttachmentInfo colorAttachment{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
	colorAttachment.imageView = _colorView;
	colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//colorAttachment.loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	//if ( clear ) colorAttachment.clearValue = *clear;

	VkRenderingAttachmentInfo depthAttachment{ .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
	depthAttachment.imageView = _depthView;
	depthAttachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	depthAttachment.clearValue.depthStencil.depth = 0.f;

	VkRenderingInfo renderInfo{ .sType = VK_STRUCTURE_TYPE_RENDERING_INFO };
	renderInfo.renderArea = VkRect2D{ VkOffset2D { 0, 0 }, VkExtent2D{ (uint32_t)_width, (uint32_t)_height } };
	renderInfo.layerCount = 1;
	renderInfo.colorAttachmentCount = 1;
	renderInfo.pColorAttachments = &colorAttachment;
	renderInfo.pDepthAttachment = &depthAttachment;
	renderInfo.pStencilAttachment = nullptr;

	vkCmdBeginRendering( m_cmd, &renderInfo );

}

void wv::CommandBuffer::endRendering()
{ 
	vkCmdEndRendering( m_cmd );
}

void wv::CommandBuffer::setViewport( float _x, float _y, float _width, float _height, float _minDepth, float _maxDepth )
{ 
	VkViewport viewport = {};
	viewport.x = _x;
	viewport.y = _y;
	viewport.width  = _width;
	viewport.height = _height;
	viewport.minDepth = _minDepth;
	viewport.maxDepth = _maxDepth;

	vkCmdSetViewport( m_cmd, 0, 1, &viewport );
}

void wv::CommandBuffer::setScissor( float _x, float _y, float _width, float _height )
{ 
	VkRect2D scissor = {};
	scissor.offset.x = _x;
	scissor.offset.y = _y;
	scissor.extent.width  = _width;
	scissor.extent.height = _height;

	vkCmdSetScissor( m_cmd, 0, 1, &scissor );
}

void wv::CommandBuffer::bindIndexBuffer( VkBuffer _buffer, VkDeviceSize _offset, VkIndexType _type )
{ 
	vkCmdBindIndexBuffer( m_cmd, _buffer, _offset, _type );
}

void wv::CommandBuffer::bindPipeline( VkPipelineBindPoint _bindPoint, VkPipeline _pipeline )
{
	vkCmdBindPipeline( m_cmd, _bindPoint, _pipeline );
}

void wv::CommandBuffer::bindDescriptorSets( VkPipelineBindPoint _bindPoint, VkPipelineLayout _layout, uint32_t _firstSet, uint32_t _descriptorSetCount, VkDescriptorSet* _descriptorSets )
{
	vkCmdBindDescriptorSets( m_cmd, _bindPoint, _layout, _firstSet, _descriptorSetCount, _descriptorSets, 0, nullptr );
}

void wv::CommandBuffer::pushConstant( VkPipelineLayout _pipelineLayout, VkShaderStageFlags _stage, uint32_t _offset, uint32_t _size, void* _data )
{ 
	vkCmdPushConstants( m_cmd, _pipelineLayout, _stage, _offset, _size, _data );
}

void wv::CommandBuffer::transitionImage( VkImage _image, VkImageLayout _currentLayout, VkImageLayout _newLayout )
{
	VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };

	imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = _currentLayout;
	imageBarrier.newLayout = _newLayout;

	VkImageAspectFlags aspectMask = ( _newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	
	imageBarrier.subresourceRange = imageSubresourceRange( aspectMask );
	imageBarrier.image = _image;

	VkDependencyInfo depInfo{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO };

	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2( m_cmd, &depInfo );
	m_imageLastKnownLayout[ _image ] = _newLayout;
}

void wv::CommandBuffer::copyImageToImage( VkImage _source, VkImage _destination, VkExtent2D _srcSize, VkExtent2D _dstSize )
{
	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2 };

	blitRegion.srcOffsets[ 1 ].x = _srcSize.width;
	blitRegion.srcOffsets[ 1 ].y = _srcSize.height;
	blitRegion.srcOffsets[ 1 ].z = 1;

	blitRegion.dstOffsets[ 1 ].x = _dstSize.width;
	blitRegion.dstOffsets[ 1 ].y = _dstSize.height;
	blitRegion.dstOffsets[ 1 ].z = 1;

	blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount = 1;
	blitRegion.srcSubresource.mipLevel = 0;

	blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount = 1;
	blitRegion.dstSubresource.mipLevel = 0;

	VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2 };
	blitInfo.dstImage = _destination;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage = _source;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.filter = VK_FILTER_LINEAR;
	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2( m_cmd, &blitInfo );
}

void wv::CommandBuffer::clearColorImage( VkImage _image, VkImageLayout _layout, VkClearColorValue* _clearValue )
{
	VkImageSubresourceRange clearRange = imageSubresourceRange( VK_IMAGE_ASPECT_COLOR_BIT );

	vkCmdClearColorImage( m_cmd, _image, _layout, _clearValue, 1, &clearRange );
}

void wv::CommandBuffer::dispatch( uint32_t _groupCountX, uint32_t _groupCountY, uint32_t _groupCountZ )
{
	vkCmdDispatch( m_cmd, _groupCountX, _groupCountY, _groupCountZ );
}

void wv::CommandBuffer::draw( uint32_t _indexCount, uint32_t _instanceCount, uint32_t _firstIndex, int32_t _vertexOffset, uint32_t _firstInstance )
{
	vkCmdDrawIndexed( m_cmd, _indexCount, _instanceCount, _firstIndex, _vertexOffset, _firstInstance );
}
