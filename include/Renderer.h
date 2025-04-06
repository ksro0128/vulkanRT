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
#include "RenderPass.h"
#include "FrameBuffer.h"
#include "Pipeline.h"
#include "CommandBuffers.h"
#include "GuiRenderer.h"

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

	VkExtent2D m_extent;
	uint32_t currentFrame = 0;

	// resources list
	std::vector<std::unique_ptr<Mesh>> m_meshList;
	std::vector<std::unique_ptr<Texture>> m_textureList;
	std::vector<Material> m_materialList;
	std::vector<Model> m_modelList;

	// descriptorset layout
	std::unique_ptr<DescriptorSetLayout> m_globalLayout;
	std::unique_ptr<DescriptorSetLayout> m_objectMaterialLayout;
	std::unique_ptr<DescriptorSetLayout> m_bindlessLayout;

	// buffer
	std::array<std::unique_ptr<UniformBuffer>, MAX_FRAMES_IN_FLIGHT> m_cameraBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_lightBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_objectInstanceBuffers;

	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_modelBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_materialBuffers;

	// descriptorset
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_globlaDescSets;
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_objectMaterialDescSets;
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_bindlessDescSets;


	// renderpass
	std::unique_ptr<RenderPass> m_gbufferRenderPass;
	std::unique_ptr<RenderPass> m_imguiRenderPass;

	// attachment
	std::vector<GbufferAttachment> m_gbufferAttachments;

	// framebuffer
	std::vector<std::unique_ptr<FrameBuffer>> m_gbufferFrameBuffers;
	std::vector<std::unique_ptr<FrameBuffer>> m_imguiFrameBuffers;

	// pipeline
	std::unique_ptr<Pipeline> m_gbufferPipeline;

	// command buffer
	std::unique_ptr<CommandBuffers> m_commandBuffers;

	// gui renderer
	std::unique_ptr<GuiRenderer> m_guiRenderer;


	// scene
	std::vector<Object> m_objects;
	glm::vec3 m_cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


	std::unique_ptr<ImageBuffer> tmp[30];
	std::shared_ptr<Mesh> tmpMesh[1000];
	std::unique_ptr<Texture> tmpTexture[30];


	void cleanup();
	void init(GLFWwindow* window);
	void recreateSwapChain();
	void recreateViewport(ImVec2 newExtent);
	
	void loadModel(const std::string& modelPath);
	void createDefaultModels();

	void recordGbufferCommandBuffer();
	void recordImGuiCommandBuffer(uint32_t imageIndex);

	void printAllResources();
	void scene();

	void transferImageLayout(VkCommandBuffer cmd, Texture* texture, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage);
};