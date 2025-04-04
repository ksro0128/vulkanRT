#pragma once

#include "Common.h"
#include "VulkanContext.h"
#include "DescriptorSetLayout.h"
#include "Texture.h"
#include "Buffer.h"

class DescriptorSet {
public:
	static std::unique_ptr<DescriptorSet> createGlobalDescriptorSet(
		VulkanContext* context,
		DescriptorSetLayout* layout,
		UniformBuffer* cameraBuffer,
		StorageBuffer* lightBuffer
	);

	static std::unique_ptr<DescriptorSet> createObjectMaterialDescriptorSet(
		VulkanContext* context,
		DescriptorSetLayout* layout,
		StorageBuffer* modelBuffer,
		StorageBuffer* materialBuffer
	);

	static std::unique_ptr<DescriptorSet> createTextureDescriptorSet(
		VulkanContext* context,
		DescriptorSetLayout* layout,
		StorageBuffer* materialBuffer,
		const std::vector<std::unique_ptr<Texture>>& textureList
	);

	VkDescriptorSet& getDescriptorSet() { return m_descriptorSet; }

	~DescriptorSet();

private:
	VulkanContext* context;
	VkDescriptorSet m_descriptorSet;

	void initGlobal(VulkanContext* context, DescriptorSetLayout* layout, 
		UniformBuffer* cameraBuffer, StorageBuffer* lightBuffer);
	void initObjectMaterial(VulkanContext* context, DescriptorSetLayout* layout,
		StorageBuffer* modelBuffer, StorageBuffer* materialBuffer);
	void initTexture(VulkanContext* context, DescriptorSetLayout* layout, StorageBuffer* materialBuffer,
		const std::vector<std::unique_ptr<Texture>>& textureList);
	void cleanup();
};
