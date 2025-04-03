#include "include/DescriptorSet.h"

DescriptorSet::~DescriptorSet() {
	cleanup();
}

void DescriptorSet::cleanup() {
	std::cout << "DescriptorSet::cleanup" << std::endl;

}

std::unique_ptr<DescriptorSet> DescriptorSet::createGlobalDescriptorSet(VulkanContext* context, DescriptorSetLayout* layout, 
	UniformBuffer* cameraBuffer, StorageBuffer* lightBuffer) {
	std::unique_ptr<DescriptorSet> descriptorSet = std::unique_ptr<DescriptorSet>(new DescriptorSet());
	descriptorSet->initGlobal(context, layout, cameraBuffer, lightBuffer);
	return descriptorSet;
}

void DescriptorSet::initGlobal(VulkanContext* context, DescriptorSetLayout* layout,
	UniformBuffer* cameraBuffer, StorageBuffer* lightBuffer)
{
	this->context = context;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = context->getDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	VkDescriptorSetLayout& vkLayout = layout->getDescriptorSetLayout();
	allocInfo.pSetLayouts = &vkLayout;

	if (vkAllocateDescriptorSets(context->getDevice(), &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate global descriptor set!");
	}

	std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

	// Binding 0: Camera (UBO)
	VkDescriptorBufferInfo cameraBufferInfo{};
	cameraBufferInfo.buffer = cameraBuffer->getBuffer();
	cameraBufferInfo.offset = 0;
	cameraBufferInfo.range = sizeof(CameraBuffer);

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = m_descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &cameraBufferInfo;

	// Binding 1: Light (SSBO)
	VkDescriptorBufferInfo lightBufferInfo{};
	lightBufferInfo.buffer = lightBuffer->getBuffer();
	lightBufferInfo.offset = 0;
	lightBufferInfo.range = lightBuffer->getCurrentSize();

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = m_descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo = &lightBufferInfo;

	vkUpdateDescriptorSets(context->getDevice(),
		static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

std::unique_ptr<DescriptorSet> DescriptorSet::createObjectMaterialDescriptorSet(VulkanContext* context, DescriptorSetLayout* layout,
	StorageBuffer* modelBuffer, StorageBuffer* materialBuffer)
{
	std::unique_ptr<DescriptorSet> descriptorSet = std::unique_ptr<DescriptorSet>(new DescriptorSet());
	descriptorSet->initObjectMaterial(context, layout, modelBuffer, materialBuffer);
	return descriptorSet;
}

void DescriptorSet::initObjectMaterial(VulkanContext* context, DescriptorSetLayout* layout,
	StorageBuffer* modelBuffer, StorageBuffer* materialBuffer)
{
	this->context = context;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = context->getDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	VkDescriptorSetLayout vkLayout = layout->getDescriptorSetLayout();
	allocInfo.pSetLayouts = &vkLayout;

	if (vkAllocateDescriptorSets(context->getDevice(), &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate object/material descriptor set!");
	}

	std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

	// Binding 0: Model SSBO
	VkDescriptorBufferInfo modelBufferInfo{};
	modelBufferInfo.buffer = modelBuffer->getBuffer();
	modelBufferInfo.offset = 0;
	modelBufferInfo.range = modelBuffer->getCurrentSize();

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = m_descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &modelBufferInfo;

	// Binding 1: Material SSBO
	VkDescriptorBufferInfo materialBufferInfo{};
	materialBufferInfo.buffer = materialBuffer->getBuffer();
	materialBufferInfo.offset = 0;
	materialBufferInfo.range = materialBuffer->getCurrentSize();

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = m_descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo = &materialBufferInfo;

	vkUpdateDescriptorSets(context->getDevice(),
		static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

std::unique_ptr<DescriptorSet> DescriptorSet::createTextureDescriptorSet(
	VulkanContext* context, DescriptorSetLayout* layout, const std::vector<std::unique_ptr<Texture>>& textureList)
{
	std::unique_ptr<DescriptorSet> descriptorSet = std::unique_ptr<DescriptorSet>(new DescriptorSet());
	descriptorSet->initTexture(context, layout, textureList);
	return descriptorSet;
}

void DescriptorSet::initTexture(VulkanContext* context, DescriptorSetLayout* layout, const std::vector<std::unique_ptr<Texture>>& textureList)
{
	this->context = context;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = context->getDescriptorPool();
	allocInfo.descriptorSetCount = 1;

	VkDescriptorSetLayout vkLayout = layout->getDescriptorSetLayout();
	allocInfo.pSetLayouts = &vkLayout;

	if (vkAllocateDescriptorSets(context->getDevice(), &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate texture descriptor set!");
	}

	// Image infos 배열 생성
	std::vector<VkDescriptorImageInfo> imageInfos;
	imageInfos.reserve(MAX_TEXTURE_COUNT);

	for (size_t i = 0; i < textureList.size(); ++i) {
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureList[i]->getImageView();
		imageInfo.sampler = textureList[i]->getSampler();

		if (imageInfo.imageView == VK_NULL_HANDLE || imageInfo.sampler == VK_NULL_HANDLE) {
			std::cerr << "[TextureDescriptor] Texture[" << i << "] has null handle! Skipping.\n";
			return;
		}
		imageInfos.push_back(imageInfo);
	}

	 // 나머지는 더미 슬롯
	while (imageInfos.size() < MAX_TEXTURE_COUNT) {
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureList[0]->getImageView();
		imageInfo.sampler = textureList[0]->getSampler();
		imageInfos.push_back(imageInfo);
	}

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = static_cast<uint32_t>(imageInfos.size());
	descriptorWrite.pImageInfo = imageInfos.data();

	vkUpdateDescriptorSets(context->getDevice(), 1, &descriptorWrite, 0, nullptr);
}
