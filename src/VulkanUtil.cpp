#include "include/VulkanUtil.h"


// �̹��� �� ����
VkImageView VulkanUtil::createImageView(VulkanContext* context, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
	// �̹��� �� ���� ����
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;													// �̹��� �ڵ�
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;								// �̹��� Ÿ��
	viewInfo.format = format;												// �̹��� ����
	viewInfo.subresourceRange.aspectMask = aspectFlags;  					// �̹��� ���� ���� (color / depth / stencil ��)
	viewInfo.subresourceRange.baseMipLevel = 0;                          	// �������� mipmap �ܰ� ����
	viewInfo.subresourceRange.levelCount = mipLevels;                       // baseMipLevel �������� �� ���� MipLevel�� �� ������� ���� (���� mipmap ����� �� ���� �������)
	viewInfo.subresourceRange.baseArrayLayer = 0;                        	// ImageView�� �����ϴ� �̹��� ���̾��� ���� ��ġ ����
	viewInfo.subresourceRange.layerCount = 1;                            	// ���� ü�ο��� ������ �̹��� ���̾� ����

	// �̹��� �� ����
	VkImageView imageView;
	if (vkCreateImageView(context->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image view!");
	}

	return imageView;
}