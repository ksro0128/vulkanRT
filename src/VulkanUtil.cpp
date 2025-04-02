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

	// 이미지 객체를 만드는데 사용되는 구조체
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;					// 이미지의 차원을 설정
	imageInfo.extent.width = width;							// 이미지의 너비 지정
	imageInfo.extent.height = height;						// 이미지의 높이 지정 
	imageInfo.extent.depth = 1;								// 이미지의 깊이 지정 (2D 이미지의 경우 depth는 1로 지정해야 함)
	imageInfo.mipLevels = mipLevels;						// 생성할 mipLevel의 개수 지정
	imageInfo.arrayLayers = 1;								// 생성할 이미지 레이어 수 (큐브맵의 경우 6개 생성)
	imageInfo.format = format;								// 이미지의 포맷을 지정하며, 채널 구성과 각 채널의 비트 수를 정의
	imageInfo.tiling = tiling;								// 이미지를 GPU 메모리에 배치할 때 메모리 레이아웃을 결정하는 설정 (CPU에서도 접근 가능하게 할꺼냐, GPU에만 접근 가능하게 최적화 할거냐 결정) 
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;	// 이미지 초기 레이아웃 설정 (이미지가 메모리에 배치될 때 초기 상태를 정의)
	imageInfo.usage = usage;								// 이미지의 사용 용도 결정
	imageInfo.samples = numSamples;							// 멀티 샘플링을 위한 샘플 개수
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		// 이미지의 큐 공유 모드 설정 (VK_SHARING_MODE_EXCLUSIVE: 한 번에 하나의 큐 패밀리에서만 접근 가능한 단일 큐 모드)

	// 이미지 객체 생성
	if (vkCreateImage(context->getDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}

	// 이미지에 필요한 메모리 요구 사항을 조회 
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(context->getDevice(), image, &memRequirements);

	// 메모리 할당을 위한 구조체
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;											// 메모리 크기 설정
	allocInfo.memoryTypeIndex = findMemoryType(context, memRequirements.memoryTypeBits, properties);		// 메모리 유형과 속성 설정

	// 이미지를 위한 메모리 할당
	if (vkAllocateMemory(context->getDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate image memory!");
	}

	// 이미지에 할당한 메모리 바인딩
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