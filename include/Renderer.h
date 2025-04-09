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
#include "Scene.h"

class Renderer {
public:
	static std::unique_ptr<Renderer> createRenderer(GLFWwindow* window);
	~Renderer();

	void update(float deltaTime);
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
	std::unique_ptr<DescriptorSetLayout> m_attachmentLayout;
	std::unique_ptr<DescriptorSetLayout> m_shadowLayout;

	// buffer
	std::array<std::unique_ptr<UniformBuffer>, MAX_FRAMES_IN_FLIGHT> m_cameraBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_lightBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_objectInstanceBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_modelBuffers;
	std::array<std::unique_ptr<StorageBuffer>, MAX_FRAMES_IN_FLIGHT> m_materialBuffers;
	std::array<std::unique_ptr<UniformBuffer>, MAX_FRAMES_IN_FLIGHT> m_lightMatrixBuffers;

	// renderpass
	std::unique_ptr<RenderPass> m_gbufferRenderPass;
	std::unique_ptr<RenderPass> m_imguiRenderPass;
	std::unique_ptr<RenderPass> m_lightPassRenderPass;
	std::unique_ptr<RenderPass> m_shadowMapRenderPass;

	// attachment
	std::vector<GbufferAttachment> m_gbufferAttachments;
	std::vector<std::unique_ptr<Texture>> m_outputTextures;
	std::vector<std::vector<std::unique_ptr<Texture>>> m_shadowMapTextures;
	std::vector<std::unique_ptr<Texture>> m_shadowCubeMapTextures;

	// framebuffer
	std::vector<std::unique_ptr<FrameBuffer>> m_gbufferFrameBuffers;
	std::vector<std::unique_ptr<FrameBuffer>> m_imguiFrameBuffers;
	std::vector<std::unique_ptr<FrameBuffer>> m_outputFrameBuffers;
	std::vector<std::vector<std::unique_ptr<FrameBuffer>>> m_shadowMapFrameBuffers;
	std::vector<std::vector<std::unique_ptr<FrameBuffer>>> m_shadowCubeMapFrameBuffers;

	// descriptorset
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_globlaDescSets;
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_objectMaterialDescSets;
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_bindlessDescSets;
	std::vector< std::unique_ptr<DescriptorSet> > m_attachmentDescSets;
	std::array<std::unique_ptr<DescriptorSet>, MAX_FRAMES_IN_FLIGHT> m_shadowDescSets;


	// pipeline
	std::unique_ptr<Pipeline> m_gbufferPipeline;
	std::unique_ptr<Pipeline> m_lightPassPipeline;
	std::unique_ptr<Pipeline> m_shadowMapPipeline;

	// command buffer
	std::unique_ptr<CommandBuffers> m_commandBuffers;

	// gui renderer
	std::unique_ptr<GuiRenderer> m_guiRenderer;

	// scene
	std::unique_ptr<Scene> m_scene;

	// camera
	Camera m_camera;
	bool m_mousePressed = false;
	double m_lastMouseX = 0.0, m_lastMouseY = 0.0;
	float m_yaw = -90.0f;
	float m_pitch = 0.0f;
	float m_mouseSensitivity = 0.2f;
	float m_moveSpeed = 3.0f;

	void cleanup();
	void init(GLFWwindow* window);
	void recreateSwapChain();
	void recreateViewport(ImVec2 newExtent);
	
	void loadModel(const std::string& modelPath);
	void createDefaultModels();

	std::unordered_map<int32_t, std::vector<int32_t>> createObjectMap();

	void recordGbufferCommandBuffer(std::unordered_map<int32_t, std::vector<int32_t>>& objectMap);
	void recordLightPassCommandBuffer();
	void recordImGuiCommandBuffer(uint32_t imageIndex);
	void recordShadowMapCommandBuffer(std::unordered_map<int32_t, std::vector<int32_t>>& objectMap);

	void printAllResources();

	void transferImageLayout(VkCommandBuffer cmd, Texture* texture, VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, uint32_t layerCount = 1);

	void updateCamera(float deltaTime);

	glm::mat4 computeLightMatrix(Light& light);
	glm::mat4 computePointLightMatrix(Light& light, uint32_t faceIndex);
};