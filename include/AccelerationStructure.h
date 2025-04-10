#pragma once

#include "Common.h"
#include "VulkanContext.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Model.h"
#include "Object.h"
#include "VulkanUtil.h"


class AccelerationStructure {
public:
	virtual ~AccelerationStructure();

	VkAccelerationStructureKHR getHandle() const { return m_as; }
	VkDeviceAddress getDeviceAddress() const { return m_deviceAddress; }
	VkBuffer getBuffer() const { return m_buffer; }

protected:
	VulkanContext* context;

	VkAccelerationStructureKHR m_as = VK_NULL_HANDLE;
	VkBuffer m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;
	VkDeviceAddress m_deviceAddress = 0;
	void cleanup();
};

class BottomLevelAS : public AccelerationStructure {
public:
	static std::unique_ptr<BottomLevelAS> createBottomLevelAS(VulkanContext* context, Mesh* mesh);
private:
	void initBLAS(VulkanContext* context, Mesh* mesh);
};

class TopLevelAS : public AccelerationStructure {
public:
	static std::unique_ptr<TopLevelAS> createTopLevelAS(VulkanContext* context, std::vector<std::unique_ptr<BottomLevelAS>>& blasList,
		std::vector<Model>& modelList, std::unordered_map<int32_t, std::vector<int32_t>>& modelToMatrixIndices,
		std::vector<ModelBuffer>& modelBuffers, std::vector<Object>& objects);
	void rebuild(std::vector<std::unique_ptr<BottomLevelAS>>& blasList,
		std::vector<Model>& modelList, std::unordered_map<int32_t, std::vector<int32_t>>& modelToMatrixIndices,
		std::vector<ModelBuffer>& modelBuffers, std::vector<Object>& objects);
private:
	void initTLAS(VulkanContext* context, std::vector<std::unique_ptr<BottomLevelAS>>& blasList,
		std::vector<Model>& modelList, std::unordered_map<int32_t, std::vector<int32_t>>& modelToMatrixIndices,
		std::vector<ModelBuffer>& modelBuffers, std::vector<Object>& objects);
};