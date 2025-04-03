#pragma once

#include "Common.h"
#include "VulkanContext.h"

class DescriptorSetLayout {
public:
	static std::unique_ptr<DescriptorSetLayout> createGlobalDescriptorSetLayout(VulkanContext* context);
	static std::unique_ptr<DescriptorSetLayout> createObjectMaterialDescriptorSetLayout(VulkanContext* context);
	static std::unique_ptr<DescriptorSetLayout> createTextureDescriptorSetLayout(VulkanContext* context);

	~DescriptorSetLayout();

	VkDescriptorSetLayout &getDescriptorSetLayout() { return m_layout; }

private:
	VulkanContext* context;
	VkDescriptorSetLayout m_layout;

	void initGlobal(VulkanContext* context);
	void initObjectMaterial(VulkanContext* context);
	void initTexture(VulkanContext* context);
	void cleanup();
};