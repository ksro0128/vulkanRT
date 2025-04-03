#include "include/Texture.h"

std::unique_ptr<Texture> Texture::createTexture(VulkanContext* context, std::string path, TextureFormatType formatType) {
	std::unique_ptr<Texture> texture = std::unique_ptr<Texture>(new Texture());
	texture->init(context, path, formatType);
	return texture;
}

void Texture::init(VulkanContext* context, std::string path, TextureFormatType formatType) {
	this->context = context;

	VkFormat format = (formatType == TextureFormatType::ColorSRGB)
		? VK_FORMAT_R8G8B8A8_SRGB
		: VK_FORMAT_R8G8B8A8_UNORM;

	m_imageBuffer = ImageBuffer::createImageBuffer(context, path, format);
	m_imageView = VulkanUtil::createImageView(context, m_imageBuffer->getImage(), format,
		VK_IMAGE_ASPECT_COLOR_BIT, m_imageBuffer->getMipLevels());
	VkSamplerCreateInfo samplerInfo = createDefaultSamplerInfo();
	if (vkCreateSampler(context->getDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
	{
		std::cerr << "failed to create image sampler" << std::endl;
	}

	std::cout << "imageview: " << m_imageView << std::endl;
	std::cout << "imageSampler: " << m_sampler << std::endl;

}

Texture::~Texture() {
	cleanup();
}

void Texture::cleanup() {
	std::cout << "Texture::cleanup" << std::endl;
	if (m_sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(context->getDevice(), m_sampler, nullptr);
		m_sampler = VK_NULL_HANDLE;
	}
	if (m_imageView != VK_NULL_HANDLE)
	{
		vkDestroyImageView(context->getDevice(), m_imageView, nullptr);
		m_imageView = VK_NULL_HANDLE;
	}
}

VkSamplerCreateInfo Texture::createDefaultSamplerInfo() {
	// GPU의 속성 정보를 가져오는 함수
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(context->getPhysicalDevice(), &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;				   // 확대시 필터링 적용 설정 (현재 선형 보간 필터링 적용)
	samplerInfo.minFilter = VK_FILTER_LINEAR;				   // 축소시 필터링 적용 설정 (현재 선형 보간 필터링 적용)

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // 텍스처 좌표계의 U축(너비)에서 범위를 벗어난 경우
															   // 래핑 모드 설정 (현재 반복 설정)
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; // 텍스처 좌표계의 V축(높이)에서 범위를 벗어난 경우
															   // 래핑 모드 설정 (현재 반복 설정)
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; // 텍스처 좌표계의 W축(깊이)에서 범위를 벗어난 경우
															   // 래핑 모드 설정 (현재 반복 설정)
	samplerInfo.anisotropyEnable =
		VK_TRUE; // 이방성 필터링 적용 여부 설정 (경사진 곳이나 먼 곳의 샘플을 늘려 좀 더 정확한 값을 가져오는 방법)
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy; // GPU가 지원하는 최대의 이방성 필터링 제한 설정
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // 래핑 모드가 VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
																// 일때 텍스처 외부 색 지정
	samplerInfo.unnormalizedCoordinates =
		VK_FALSE; // VK_TRUE로 설정시 텍스처 좌표가 0 ~ 1의 정규화된 좌표가 아닌 실제 텍셀의 좌표 범위로 바뀜
	samplerInfo.compareEnable =
		VK_FALSE; // 비교 연산 사용할지 결정 (보통 쉐도우 맵같은 경우 깊이 비교 샘플링에서 사용됨)
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;			// 비교 연산에 사용할 연산 지정
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // mipmap 사용시 mipmap 간 보간 방식 결정 (현재 선형 보간)
	samplerInfo.minLod = 0.0f; // 최소 level을 0으로 설정 (가장 높은 해상도의 mipmap 을 사용가능하게 허용)
	samplerInfo.maxLod = static_cast<float>(
		m_imageBuffer->getMipLevels()); // 최대 level을 mipLevel로 설정 (VK_LOD_CLAMP_NONE 설정시 제한 해제)
	samplerInfo.mipLodBias =
		0.0f; // Mipmap 레벨 오프셋(Bias)을 설정
			  // Mipmap을 일부러 더 높은(더 큰) 레벨로 사용하거나 낮은(더 작은) 레벨로 사용하고 싶을 때 사용.

	return samplerInfo;
}

std::unique_ptr<Texture> Texture::createDefaultTexture(VulkanContext* context, glm::vec4 color) {
	std::unique_ptr<Texture> texture = std::unique_ptr<Texture>(new Texture());
	texture->initDefaultTexture(context, color);
	return texture;
}

void Texture::initDefaultTexture(VulkanContext* context, glm::vec4 color) {
	this->context = context;

	m_imageBuffer = ImageBuffer::createDefaultImageBuffer(context, color);

	m_imageView = VulkanUtil::createImageView(
		context,
		m_imageBuffer->getImage(),
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_ASPECT_COLOR_BIT,
		m_imageBuffer->getMipLevels()
	);

	VkSamplerCreateInfo samplerInfo = createDefaultSamplerInfo();
	if (vkCreateSampler(context->getDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create default image sampler!");
	}

	std::cout << "imageview: " << m_imageView << std::endl;
	std::cout << "imageSampler: " << m_sampler << std::endl;
}