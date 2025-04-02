#pragma once

#include "Common.h"
#include "Buffer.h"

class Mesh {
public:
	static std::shared_ptr<Mesh> createMesh(VulkanContext* context, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	static std::shared_ptr<Mesh> createBoxMesh(VulkanContext* context);
	static std::shared_ptr<Mesh> createSphereMesh(VulkanContext* context);
	static std::shared_ptr<Mesh> createPlaneMesh(VulkanContext* context);

	~Mesh();
	void draw(VkCommandBuffer commandBuffer);
	uint32_t getID() { return m_id; }

private:
	VulkanContext* context;
	std::unique_ptr<VertexBuffer> m_vertexBuffer;
	std::unique_ptr<IndexBuffer> m_indexBuffer;
	uint32_t m_id;

	void init(VulkanContext* context, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	void cleanup();
	void Mesh::calculateTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
};