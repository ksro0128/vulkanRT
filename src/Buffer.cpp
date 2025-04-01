#include "include/Buffer.h"

void Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
					  VkDeviceMemory &bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; 
	if (vkCreateBuffer(context->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(context->getDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VulkanUtil::findMemoryType(context, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(context->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(context->getDevice(), buffer, bufferMemory, 0);
}

void Buffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBuffer commandBuffer = VulkanUtil::beginSingleTimeCommands(context);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	VulkanUtil::endSingleTimeCommands(context, commandBuffer);	
}

// vertex buffer
std::unique_ptr<VertexBuffer> VertexBuffer::createVertexBuffer(VulkanContext* context, std::vector<Vertex> &vertices) {
	std::unique_ptr<VertexBuffer> buffer = std::unique_ptr<VertexBuffer>(new VertexBuffer());
	buffer->init(context, vertices);
	return buffer;
}

void VertexBuffer::init(VulkanContext* context, std::vector<Vertex> &vertices) {
	this->context = context;
	
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
				 stagingBufferMemory);

	void *data;
	vkMapMemory(context->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);

	vkUnmapMemory(context->getDevice(), stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_buffer, m_bufferMemory);
	copyBuffer(stagingBuffer, m_buffer, bufferSize);

	vkDestroyBuffer(context->getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(context->getDevice(), stagingBufferMemory, nullptr);
}

void VertexBuffer::bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = {m_buffer};
	VkDeviceSize offsets[] = {0};
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
}

VertexBuffer::~VertexBuffer() {
	cleanup();
}

void VertexBuffer::cleanup() {
	vkDestroyBuffer(context->getDevice(), m_buffer, nullptr);
	vkFreeMemory(context->getDevice(), m_bufferMemory, nullptr);
}





