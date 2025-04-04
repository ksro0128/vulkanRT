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

	std::vector<VkDescriptorSetLayoutBinding> bindings;

	VkDescriptorSetLayoutBinding materialBufferBinding{};
	materialBufferBinding.binding = 0;
	materialBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	materialBufferBinding.descriptorCount = 1;
	materialBufferBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	materialBufferBinding.pImmutableSamplers = nullptr;
	bindings.push_back(materialBufferBinding);

	VkDescriptorSetLayoutBinding textureBinding{};
	textureBinding.binding = 1;
	textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	textureBinding.descriptorCount = MAX_TEXTURE_COUNT;
	textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	textureBinding.pImmutableSamplers = nullptr;
	bindings.push_back(textureBinding);

	std::vector<VkDescriptorBindingFlags> bindingFlags = {
		0,
		VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT
	};

	VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsInfo{};
	bindingFlagsInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	bindingFlagsInfo.bindingCount = static_cast<uint32_t>(bindingFlags.size());
	bindingFlagsInfo.pBindingFlags = bindingFlags.data();

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
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