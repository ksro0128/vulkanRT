#pragma once

#include "Common.h"
#include "Buffer.h"

enum class TextureFormatType {
	ColorSRGB,     // Albedo, Emissive
	LinearUNORM    // Normal, Roughness, Metallic, AO
};

class Texture {
public:
	static std::unique_ptr<Texture> createTexture(VulkanContext* context, std::string path, TextureFormatType formatType);
	static std::unique_ptr<Texture> createDefaultTexture(VulkanContext *context, glm::vec4 color);
	~Texture();

	VkImageView getImageView() { return m_imageView; }
	VkSampler getSampler() { return m_sampler; }

private:
	VulkanContext* context;
	std::unique_ptr<ImageBuffer> m_imageBuffer;
	VkImageView m_imageView;
	VkSampler m_sampler;

	void init(VulkanContext* context, std::string path, TextureFormatType formatType);
	void initDefaultTexture(VulkanContext* context, glm::vec4 color);
	void cleanup();

	VkSamplerCreateInfo createDefaultSamplerInfo();
};