#include "include/VulkanUtil.h"


// 이미지 뷰 생성
VkImageView VulkanUtil::createImageView(VulkanContext* context, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) {
	// 이미지 뷰 정보 생성
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;													// 이미지 핸들
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;								// 이미지 타입
	viewInfo.format = format;												// 이미지 포맷
	viewInfo.subresourceRange.aspectMask = aspectFlags;  					// 이미지 형식 결정 (color / depth / stencil 등)
	viewInfo.subresourceRange.baseMipLevel = 0;                          	// 렌더링할 mipmap 단계 설정
	viewInfo.subresourceRange.levelCount = mipLevels;                       // baseMipLevel 기준으로 몇 개의 MipLevel을 더 사용할지 설정 (실제 mipmap 만드는 건 따로 해줘야함)
	viewInfo.subresourceRange.baseArrayLayer = 0;                        	// ImageView가 참조하는 이미지 레이어의 시작 위치 정의
	viewInfo.subresourceRange.layerCount = 1;                            	// 스왑 체인에서 설정한 이미지 레이어 개수

	// 이미지 뷰 생성
	VkImageView imageView;
	if (vkCreateImageView(context->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image view!");
	}

	return imageView;
}