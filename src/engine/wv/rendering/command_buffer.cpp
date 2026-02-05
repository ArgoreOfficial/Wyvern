#include "command_buffer.h"

///////////////////////////////////////////////////////////////////////////////////////

wv::CommandBuffer::CommandBuffer( VkDevice _device, VkCommandPool _pool )
{ 
	VkCommandBufferAllocateInfo cmdAllocInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	cmdAllocInfo.commandPool = _pool;
	cmdAllocInfo.commandBufferCount = 1;
	cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	vkAllocateCommandBuffers( _device, &cmdAllocInfo, &m_cmd );
}

void wv::CommandBuffer::submit( VkQueue _queue, VkSemaphoreSubmitInfo* _waitInfo, VkSemaphoreSubmitInfo* _signalInfo, VkFence _fence )
{
	VkCommandBufferSubmitInfo cmdInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO };
	cmdInfo.commandBuffer = m_cmd;

	VkSubmitInfo2 submitInfo = makeVkSubmitInfo2( 1, &cmdInfo, _signalInfo ? 1 : 0, _signalInfo, _waitInfo ? 1 : 0, _waitInfo );
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

void wv::CommandBuffer::transitionImage( VkImage _image, VkImageLayout _oldLayout, VkImageLayout _newLayout )
{
	VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
	imageBarrier.oldLayout = _oldLayout;
	imageBarrier.newLayout = _newLayout;
	imageBarrier.image = _image;
	imageBarrier.subresourceRange = makeVkImageSubresourceRange( ( _newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL ) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT );

	imageBarrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;
	
	VkDependencyInfo depInfo{ .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO };
	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageBarrier;
	vkCmdPipelineBarrier2( m_cmd, &depInfo );
}

void wv::CommandBuffer::copyImageToImage( VkImage _src, VkImage _dst, VkExtent2D _srcSize, VkExtent2D _dstSize )
{
	VkImageBlit2 blitRegion{ .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2 };
	blitRegion.srcOffsets[ 1 ] = { (int)_srcSize.width, (int)_srcSize.height, 1 };
	blitRegion.dstOffsets[ 1 ] = { (int)_dstSize.width, (int)_dstSize.height, 1 };
	
	blitRegion.srcSubresource = makeVkImageSubresourceLayers( VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 );
	blitRegion.dstSubresource = makeVkImageSubresourceLayers( VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 );
	
	VkBlitImageInfo2 blitInfo{ .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2 };
	blitInfo.srcImage       = _src;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.dstImage       = _dst;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.filter         = VK_FILTER_LINEAR;

	blitInfo.regionCount = 1;
	blitInfo.pRegions = &blitRegion;

	vkCmdBlitImage2( m_cmd, &blitInfo );
}