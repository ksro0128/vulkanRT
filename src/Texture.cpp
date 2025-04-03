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
	// GPU�� �Ӽ� ������ �������� �Լ�
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(context->getPhysicalDevice(), &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;				   // Ȯ��� ���͸� ���� ���� (���� ���� ���� ���͸� ����)
	samplerInfo.minFilter = VK_FILTER_LINEAR;				   // ��ҽ� ���͸� ���� ���� (���� ���� ���� ���͸� ����)

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // �ؽ�ó ��ǥ���� U��(�ʺ�)���� ������ ��� ���
															   // ���� ��� ���� (���� �ݺ� ����)
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; // �ؽ�ó ��ǥ���� V��(����)���� ������ ��� ���
															   // ���� ��� ���� (���� �ݺ� ����)
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; // �ؽ�ó ��ǥ���� W��(����)���� ������ ��� ���
															   // ���� ��� ���� (���� �ݺ� ����)
	samplerInfo.anisotropyEnable =
		VK_TRUE; // �̹漺 ���͸� ���� ���� ���� (����� ���̳� �� ���� ������ �÷� �� �� ��Ȯ�� ���� �������� ���)
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy; // GPU�� �����ϴ� �ִ��� �̹漺 ���͸� ���� ����
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // ���� ��尡 VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
																// �϶� �ؽ�ó �ܺ� �� ����
	samplerInfo.unnormalizedCoordinates =
		VK_FALSE; // VK_TRUE�� ������ �ؽ�ó ��ǥ�� 0 ~ 1�� ����ȭ�� ��ǥ�� �ƴ� ���� �ؼ��� ��ǥ ������ �ٲ�
	samplerInfo.compareEnable =
		VK_FALSE; // �� ���� ������� ���� (���� ������ �ʰ��� ��� ���� �� ���ø����� ����)
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;			// �� ���꿡 ����� ���� ����
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // mipmap ���� mipmap �� ���� ��� ���� (���� ���� ����)
	samplerInfo.minLod = 0.0f; // �ּ� level�� 0���� ���� (���� ���� �ػ��� mipmap �� ��밡���ϰ� ���)
	samplerInfo.maxLod = static_cast<float>(
		m_imageBuffer->getMipLevels()); // �ִ� level�� mipLevel�� ���� (VK_LOD_CLAMP_NONE ������ ���� ����)
	samplerInfo.mipLodBias =
		0.0f; // Mipmap ���� ������(Bias)�� ����
			  // Mipmap�� �Ϻη� �� ����(�� ū) ������ ����ϰų� ����(�� ����) ������ ����ϰ� ���� �� ���.

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