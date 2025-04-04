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

class IndexBuffer : public Buffer {
public:
	static std::unique_ptr<IndexBuffer> createIndexBuffer(VulkanContext* context, std::vector<uint32_t>& indices);
    ~IndexBuffer();
	void cleanup() override;
	void bind(VkCommandBuffer commandBuffer);
	uint32_t getIndexCount() { return m_indexCount; }
private:
	uint32_t m_indexCount;

	void init(VulkanContext* context, std::vector<uint32_t>& indices);
};

class ImageBuffer : public Buffer {
public:
	static std::unique_ptr<ImageBuffer> createImageBuffer(VulkanContext* context, std::string path, VkFormat format);
	static std::unique_ptr<ImageBuffer> createHDRImageBuffer(VulkanContext* context, std::string path);
	static std::unique_ptr<ImageBuffer> createDefaultImageBuffer(VulkanContext* context, glm::vec4 color);
	static std::unique_ptr<ImageBuffer> createAttachmentImageBuffer(VulkanContext* context, uint32_t width,
		uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectFlags);
	~ImageBuffer();
	void cleanup() override;

	uint32_t getMipLevels() { return m_mipLevels; }
	VkImage getImage() { return m_image; }
	VkDeviceMemory getImageMemory() { return m_textureImageMemory; }

private:
	uint32_t m_mipLevels;
	VkImage m_image;
	VkDeviceMemory m_textureImageMemory;

	bool init(VulkanContext* context, std::string path, VkFormat format);
	bool initHDR(VulkanContext* context, std::string path);
	void initDefault(VulkanContext* context, glm::vec4 color);
	void initAttachment(VulkanContext* context, uint32_t width,
		uint32_t height, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectFlags);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
};

class UniformBuffer : public Buffer {
public:
	static std::unique_ptr<UniformBuffer> createUniformBuffer(VulkanContext* context, VkDeviceSize buffersize);
	~UniformBuffer();
	void updateUniformBuffer(void* data, VkDeviceSize size);
	VkBuffer getBuffer() { return m_buffer; }
	VkDeviceMemory getBufferMemory() { return m_bufferMemory; }
private:
	void* m_mappedMemory = nullptr;

	void init(VulkanContext* context, VkDeviceSize buffersize);
	void cleanup();
};

class StorageBuffer : public Buffer {
public:
	static std::unique_ptr<StorageBuffer> createStorageBuffer(VulkanContext* context, VkDeviceSize buffersize);
	~StorageBuffer();
	VkBuffer getBuffer() { return m_buffer; }
	VkDeviceMemory getBufferMemory() { return m_bufferMemory; }
	VkDeviceSize getCurrentSize() { return m_currentSize; }

	void updateStorageBuffer(void* data, VkDeviceSize totalSize);
	void updateStorageBufferAt(uint32_t index, void* data, VkDeviceSize structSize);
	void resizeStorageBuffer(VkDeviceSize size);


private:
	void* m_mappedMemory = nullptr;
	VkDeviceSize m_currentSize = 0;

	void init(VulkanContext* context, VkDeviceSize buffersize);
	void cleanup();
};