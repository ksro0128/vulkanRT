#pragma once

#include "Common.h"
#include "VulkanUtil.h"

class Buffer {
public:
    virtual ~Buffer() {};
    virtual void cleanup() = 0;

protected:
    VulkanContext* context;
    VkBuffer m_buffer;
    VkDeviceMemory m_bufferMemory;

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
					  VkDeviceMemory &bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

class VertexBuffer : public Buffer {
public:
    static std::unique_ptr<VertexBuffer> createVertexBuffer(VulkanContext* context, std::vector<Vertex> &vertices);
    ~VertexBuffer();
    void cleanup() override;
    void bind(VkCommandBuffer commandBuffer);
private:
    void init(VulkanContext* context, std::vector<Vertex> &vertices);
};

