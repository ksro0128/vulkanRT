#include "include/DescriptorSetLayout.h"

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::createGlobalDescriptorSetLayout(VulkanContext* context) {
	std::unique_ptr<DescriptorSetLayout> layout = std::unique_ptr<DescriptorSetLayout>(new DescriptorSetLayout());
	layout->initGlobal(context);
	return layout;
}

void DescriptorSetLayout::initGlobal(VulkanContext* context) {
	this->context = context;

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	VkDescriptorSetLayoutBinding cameraBinding{};
	cameraBinding.binding = 0;
	cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	cameraBinding.descriptorCount = 1;
	cameraBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	cameraBinding.pImmutableSamplers = nullptr;
	bindings.push_back(cameraBinding);

	VkDescriptorSetLayoutBinding lightBinding{};
	lightBinding.binding = 1;
	lightBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	lightBinding.descriptorCount = 1;
	lightBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	lightBinding.pImmutableSamplers = nullptr;
	bindings.push_back(lightBinding);

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(context->getDevice(), &layoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::createObjectMaterialDescriptorSetLayout(VulkanContext* context) {
	std::unique_ptr<DescriptorSetLayout> layout = std::unique_ptr<DescriptorSetLayout>(new DescriptorSetLayout());
	layout->initObjectMaterial(context);
	return layout;
}

void DescriptorSetLayout::initObjectMaterial(VulkanContext* context) {
	this->context = context;

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	VkDescriptorSetLayoutBinding modelMatrixBinding{};
	modelMatrixBinding.binding = 0;
	modelMatrixBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	modelMatrixBinding.descriptorCount = 1;
	modelMatrixBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	modelMatrixBinding.pImmutableSamplers = nullptr;
	bindings.push_back(modelMatrixBinding);

	VkDescriptorSetLayoutBinding materialBinding{};
	materialBinding.binding = 1;
	materialBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	materialBinding.descriptorCount = 1;
	materialBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	materialBinding.pImmutableSamplers = nullptr;
	bindings.push_back(materialBinding);

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(context->getDevice(), &layoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create Set1 descriptor set layout!");
	}
}

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::createTextureDescriptorSetLayout(VulkanContext* context) {
	std::unique_ptr<DescriptorSetLayout> layout = std::unique_ptr<DescriptorSetLayout>(new DescriptorSetLayout());
	layout->initTexture(context);
	return layout;
}

void DescriptorSetLayout::initTexture(VulkanContext* context) {
	this->context = context;

	VkDescriptorSetLayoutBinding textureBinding{};
	textureBinding.binding = 0;
	textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureBinding.descriptorCount = MAX_TEXTURE_COUNT;
	textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	textureBinding.pImmutableSamplers = nullptr;

	VkDescriptorBindingFlags bindingFlags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
		VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

	VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{};
	bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	bindingFlagsInfo.bindingCount = 1;
	bindingFlagsInfo.pBindingFlags = &bindingFlags;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &textureBinding;
	layoutInfo.pNext = &bindingFlagsInfo;
	layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

	if (vkCreateDescriptorSetLayout(context->getDevice(), &layoutInfo, nullptr, &m_layout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture descriptor set layout!");
	}
}

DescriptorSetLayout::~DescriptorSetLayout() {
	cleanup();
}

void DescriptorSetLayout::cleanup() {
	std::cout << "DescriptorSetLayout::cleanup" << std::endl;
	if (m_layout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(context->getDevice(), m_layout, nullptr);
		m_layout = VK_NULL_HANDLE;
	}
}