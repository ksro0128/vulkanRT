#pragma once

#include "Common.h"
#include "Buffer.h"

class Mesh {
public:
	static std::unique_ptr<Mesh> createMesh(VulkanContext* context, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	static std::unique_ptr<Mesh> createBoxMesh(VulkanContext* context);
	static std::unique_ptr<Mesh> createSphereMesh(VulkanContext* context);
	static std::unique_ptr<Mesh> createPlaneMesh(VulkanContext* context);

	~Mesh();
	void draw(VkCommandBuffer commandBuffer);

private:
	VulkanContext* context;
	std::unique_ptr<VertexBuffer> m_vertexBuffer;
	std::unique_ptr<IndexBuffer> m_indexBuffer;

	void init(VulkanContext* context, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	void cleanup();
	void Mesh::calculateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
};