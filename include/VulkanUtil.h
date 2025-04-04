#pragma once

#include "Common.h"
#include "VulkanContext.h"

class VulkanUtil {
public:
	static void createImage(VulkanContext* context, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	static uint32_t findMemoryType(VulkanContext* context, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	static VkImageView createImageView(VulkanContext* context, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	static VkCommandBuffer beginSingleTimeCommands(VulkanContext* context);
	static void endSingleTimeCommands(VulkanContext* context, VkCommandBuffer commandBuffer);
	
	static VkFormat findDepthFormat(VulkanContext* context);
	static VkFormat findSupportedFormat(VulkanContext* context, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	static std::vector<char> readFile(const std::string& filename);
};
