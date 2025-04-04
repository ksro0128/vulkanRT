#pragma once

#include "Common.h"
#include "VulkanContext.h"
#include "SwapChain.h"
#include "SyncObjects.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "ModelLoader.h"
#include "Object.h"
#include "DescriptorSetLayout.h"
#include "DescriptorSet.h"

class Renderer {
public:
	static std::unique_ptr<Renderer> createRenderer(GLFWwindow* window);
	~Renderer();

	void update();
	void render();
private:
	GLFWwindow* window;
	std::unique_ptr<VulkanContext> m_context;
	std::unique_ptr<SwapChain> m_swapChain;
	std::unique_ptr<SyncObjects> m_syncObjects;

	// resources list
	std::vector<std::unique_ptr<Mesh>> m_meshList;
	std::vector<std::unique_ptr<Texture>> m_textureList;
	std::vector<Material> m_materialList;
	std::vector<Model> m_modelList;

	// descriptorset layout
	std::unique_ptr<DescriptorSetLayout> m_globalLayout;
	std::unique_ptr<DescriptorSetLayout> m_objectMaterialLayout;
	std::unique_ptr<DescriptorSetLayout> m_textureLayout;

	// buffer
	std::array<std::unique_ptr<UniformBuffer>, MAX_FRAMES_IN_FLIGHT> m_cameraBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_lightBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_modelBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_materialIndexBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_materialBuffers;

	// descriptorset
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_globlaDescSets;
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_objectMaterialDescSets;
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_textureDescSets;

	std::unique_ptr<ImageBuffer> tmp[30];
	std::shared_ptr<Mesh> tmpMesh[1000];
	std::unique_ptr<Texture> tmpTexture[30];


	void cleanup();
	void init(GLFWwindow* window);
	void loadModel(const std::string& modelPath);
	void createDefaultModels();
	void printAllResources();
};