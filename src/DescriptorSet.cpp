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
	StorageBuffer* objectInstanceBuffer)
{
	std::unique_ptr<DescriptorSet> descriptorSet = std::unique_ptr<DescriptorSet>(new DescriptorSet());
	descriptorSet->initObjectMaterial(context, layout, objectInstanceBuffer);
	return descriptorSet;
}

void DescriptorSet::initObjectMaterial(VulkanContext* context, DescriptorSetLayout* layout,
	StorageBuffer* objectInstanceBuffer)
{
	this->context = context;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = context->getDescriptorPool();
	allocInfo.descriptorSetCount = 1;

	VkDescriptorSetLayout vkLayout = layout->getDescriptorSetLayout();
	allocInfo.pSetLayouts = &vkLayout;

	if (vkAllocateDescriptorSets(context->getDevice(), &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate object descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = objectInstanceBuffer->getBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = objectInstanceBuffer->getCurrentSize();

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(context->getDevice(), 1, &descriptorWrite, 0, nullptr);
}

std::unique_ptr<DescriptorSet> DescriptorSet::createBindlessDescriptorSet(
	VulkanContext* context, DescriptorSetLayout* layout, StorageBuffer* modelBuffer, StorageBuffer* materialBuffer,
	const std::vector<std::unique_ptr<Texture>>& textureList)
{
	std::unique_ptr<DescriptorSet> descriptorSet = std::unique_ptr<DescriptorSet>(new DescriptorSet());
	descriptorSet->initBindless(context, layout, modelBuffer, materialBuffer, textureList);
	return descriptorSet;
}

void DescriptorSet::initBindless(VulkanContext* context, DescriptorSetLayout* layout, StorageBuffer* modelBuffer, StorageBuffer* materialBuffer,
	const std::vector<std::unique_ptr<Texture>>& textureList)
{
	this->context = context;

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = context->getDescriptorPool();
	allocInfo.descriptorSetCount = 1;
	VkDescriptorSetLayout vkLayout = layout->getDescriptorSetLayout();
	allocInfo.pSetLayouts = &vkLayout;

	if (vkAllocateDescriptorSets(context->getDevice(), &allocInfo, &m_descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate bindless descriptor set!");
	}

	VkDescriptorBufferInfo modelBufferInfo{};
	modelBufferInfo.buffer = modelBuffer->getBuffer();
	modelBufferInfo.offset = 0;
	modelBufferInfo.range = modelBuffer->getCurrentSize();

	VkWriteDescriptorSet modelWrite{};
	modelWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	modelWrite.dstSet = m_descriptorSet;
	modelWrite.dstBinding = 0;
	modelWrite.dstArrayElement = 0;
	modelWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	modelWrite.descriptorCount = 1;
	modelWrite.pBufferInfo = &modelBufferInfo;

	VkDescriptorBufferInfo materialBufferInfo{};
	materialBufferInfo.buffer = materialBuffer->getBuffer();
	materialBufferInfo.offset = 0;
	materialBufferInfo.range = materialBuffer->getCurrentSize();

	VkWriteDescriptorSet materialWrite{};
	materialWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	materialWrite.dstSet = m_descriptorSet;
	materialWrite.dstBinding = 1;
	materialWrite.dstArrayElement = 0;
	materialWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	materialWrite.descriptorCount = 1;
	materialWrite.pBufferInfo = &materialBufferInfo;

	std::vector<VkDescriptorImageInfo> imageInfos;
	imageInfos.reserve(MAX_TEXTURE_COUNT);

	for (size_t i = 0; i < textureList.size(); ++i) {
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureList[i]->getImageView();
		imageInfo.sampler = textureList[i]->getSampler();

		if (imageInfo.imageView == VK_NULL_HANDLE || imageInfo.sampler == VK_NULL_HANDLE) {
			std::cerr << "[BindlessDescriptor] Texture[" << i << "] has null handle! Skipping.\n";
			return;
		}
		imageInfos.push_back(imageInfo);
	}

	while (imageInfos.size() < MAX_TEXTURE_COUNT) {
		VkDescriptorImageInfo dummy{};
		dummy.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		dummy.imageView = textureList[0]->getImageView();
		dummy.sampler = textureList[0]->getSampler();
		imageInfos.push_back(dummy);
	}

	VkWriteDescriptorSet textureWrite{};
	textureWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	textureWrite.dstSet = m_descriptorSet;
	textureWrite.dstBinding = 2;
	textureWrite.dstArrayElement = 0;
	textureWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureWrite.descriptorCount = static_cast<uint32_t>(imageInfos.size());
	textureWrite.pImageInfo = imageInfos.data();

	std::array<VkWriteDescriptorSet, 3> writes = { modelWrite, materialWrite, textureWrite };

	vkUpdateDescriptorSets(context->getDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
}
