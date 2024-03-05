#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan_core.h>

struct sQueueFamilyIndices 
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;

	bool isComplete() { return graphics_family.has_value() && present_family.has_value(); }
};

struct sSwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};