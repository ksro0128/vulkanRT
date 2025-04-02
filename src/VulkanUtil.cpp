#include "include/VulkanUtil.h"

uint32_t VulkanUtil::findMemoryType(VulkanContext* context, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(context->getPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanUtil::createImage(VulkanContext* context, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples,
	VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {

	// �̹��� ��ü�� ����µ� ���Ǵ� ����ü
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;					// �̹����� ������ ����
	imageInfo.extent.width = width;							// �̹����� �ʺ� ����
	imageInfo.extent.height = height;						// �̹����� ���� ���� 
	imageInfo.extent.depth = 1;								// �̹����� ���� ���� (2D �̹����� ��� depth�� 1�� �����ؾ� ��)
	imageInfo.mipLevels = mipLevels;						// ������ mipLevel�� ���� ����
	imageInfo.arrayLayers = 1;								// ������ �̹��� ���̾� �� (ť����� ��� 6�� ����)
	imageInfo.format = format;								// �̹����� ������ �����ϸ�, ä�� ������ �� ä���� ��Ʈ ���� ����
	imageInfo.tiling = tiling;								// �̹����� GPU �޸𸮿� ��ġ�� �� �޸� ���̾ƿ��� �����ϴ� ���� (CPU������ ���� �����ϰ� �Ҳ���, GPU���� ���� �����ϰ� ����ȭ �Ұų� ����) 
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;	// �̹��� �ʱ� ���̾ƿ� ���� (�̹����� �޸𸮿� ��ġ�� �� �ʱ� ���¸� ����)
	imageInfo.usage = usage;								// �̹����� ��� �뵵 ����
	imageInfo.samples = numSamples;							// ��Ƽ ���ø��� ���� ���� ����
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		// �̹����� ť ���� ��� ���� (VK_SHARING_MODE_EXCLUSIVE: �� ���� �ϳ��� ť �йи������� ���� ������ ���� ť ���)

	// �̹��� ��ü ����
	if (vkCreateImage(context->getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	// �̹����� �ʿ��� �޸� �䱸 ������ ��ȸ 
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(context->getDevice(), image, &memRequirements);

	// �޸� �Ҵ��� ���� ����ü
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;											// �޸� ũ�� ����
	allocInfo.memoryTypeIndex = findMemoryType(context, memRequirements.memoryTypeBits, properties);		// �޸� ������ �Ӽ� ����

	// �̹����� ���� �޸� �Ҵ�
	if (vkAllocateMemory(context->getDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	// �̹����� �Ҵ��� �޸� ���ε�
	vkBindImageMemory(context->getDevice(), image, imageMemory, 0);
}


VkImageView VulkanUtil::createImageView(VulkanContext* context, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(context->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image view!");
	}

	return imageView;
}

VkCommandBuffer VulkanUtil::beginSingleTimeCommands(VulkanContext* context) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = context->getCommandPool();
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(context->getDevice(), &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanUtil::endSingleTimeCommands(VulkanContext* context, VkCommandBuffer commandBuffer) {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(context->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(context->getGraphicsQueue());

	vkFreeCommandBuffers(context->getDevice(), context->getCommandPool(), 1, &commandBuffer);
}