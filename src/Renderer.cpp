#include "include/Renderer.h"

std::unique_ptr<Renderer> Renderer::createRenderer(GLFWwindow* window) {
	std::unique_ptr<Renderer> renderer = std::unique_ptr<Renderer>(new Renderer());
	renderer->init(window);
	return renderer;
}

Renderer::~Renderer() {
	cleanup();
}

void Renderer::cleanup() {
	std::cout << "Renderer::cleanup" << std::endl;
	vkDeviceWaitIdle(m_context->getDevice());
}

void Renderer::init(GLFWwindow* window) {
	std::cout << "Renderer::init" << std::endl;
	this->window = window;
	m_context = VulkanContext::createVulkanContext(window);
	m_swapChain = SwapChain::createSwapChain(window, m_context.get());
	m_syncObjects = SyncObjects::createSyncObjects(m_context.get());
	m_commandBuffers = CommandBuffers::createCommandBuffers(m_context.get());

	// m_extent = m_swapChain->getSwapChainExtent();
	m_extent = {1024, 1024};

	createDefaultModels();

	// loadModel("./assets/models/lion_head_1k.gltf/lion_head_1k.gltf");
	// loadModel("./assets/models/Camera_01_1k.gltf/Camera_01_1k.gltf");
	loadModel("./assets/materials/aerial_grass_rock_1k.gltf/aerial_grass_rock_1k.gltf");
	loadModel("./assets/materials/aerial_rocks_02_1k.gltf/aerial_rocks_02_1k.gltf");
	loadModel("./assets/materials/asphalt_02_1k.gltf/asphalt_02_1k.gltf");
	loadModel("./assets/materials/beige_wall_001_1k.gltf/beige_wall_001_1k.gltf");
	loadModel("./assets/materials/brick_wall_13_4k.gltf/brick_wall_13_4k.gltf");
	loadModel("./assets/materials/brown_mud_leaves_01_1k.gltf/brown_mud_leaves_01_1k.gltf");
	loadModel("./assets/materials/plywood_1k.gltf/plywood_1k.gltf");
	loadModel("./assets/materials/fabric_pattern_07_1k.gltf/fabric_pattern_07_1k.gltf");



	// descriptorset layout
	m_globalLayout = DescriptorSetLayout::createGlobalDescriptorSetLayout(m_context.get());
	m_objectMaterialLayout = DescriptorSetLayout::createObjectMaterialDescriptorSetLayout(m_context.get());
	m_bindlessLayout = DescriptorSetLayout::createBindlessDescriptorSetLayout(m_context.get());


	// buffers
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_cameraBuffers[i] = UniformBuffer::createUniformBuffer(m_context.get(), sizeof(CameraBuffer));
		m_lightBuffers[i] = StorageBuffer::createStorageBuffer(m_context.get(), sizeof(LightBuffer));
		m_objectInstanceBuffers[i] = StorageBuffer::createStorageBuffer(m_context.get(), sizeof(ObjectInstance) * MAX_OBJECT_COUNT * 2);
		m_modelBuffers[i] = StorageBuffer::createStorageBuffer(m_context.get(), sizeof(ModelBuffer) * MAX_OBJECT_COUNT);
		m_materialBuffers[i] = StorageBuffer::createStorageBuffer(m_context.get(), sizeof(Material) * MAX_MATERIAL_COUNT * 2);
	}

	// descriptorset
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_globlaDescSets[i] = DescriptorSet::createGlobalDescriptorSet(m_context.get(), m_globalLayout.get(), m_cameraBuffers[i].get(), m_lightBuffers[i].get());
		m_objectMaterialDescSets[i] = DescriptorSet::createObjectMaterialDescriptorSet(m_context.get(), m_objectMaterialLayout.get(), m_objectInstanceBuffers[i].get());
		m_bindlessDescSets[i] = DescriptorSet::createBindlessDescriptorSet(m_context.get(), m_bindlessLayout.get(), m_modelBuffers[i].get(), m_materialBuffers[i].get(), m_textureList);
	}

	//renderpass
	m_gbufferRenderPass = RenderPass::createGbufferRenderPass(m_context.get());
	m_imguiRenderPass = RenderPass::createImGuiRenderPass(m_context.get(), m_swapChain.get());

	// attachment
	m_gbufferAttachments.resize(MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_gbufferAttachments[i].albedo = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].normal = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].position = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].pbr = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].depth = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	m_gbufferFrameBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	// framebuffer
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_gbufferFrameBuffers[i] = FrameBuffer::createGbufferFrameBuffer(m_context.get(), m_gbufferRenderPass.get(), m_gbufferAttachments[i], m_extent);
	}

	m_imguiFrameBuffers.resize(m_swapChain->getSwapChainImages().size());
	for (int i = 0; i < m_swapChain->getSwapChainImages().size(); i++) {
		m_imguiFrameBuffers[i] = FrameBuffer::createImGuiFrameBuffer(m_context.get(), m_imguiRenderPass.get(), m_swapChain->getSwapChainImageViews()[i], m_swapChain->getSwapChainExtent());
	}

	// pipeline
	m_gbufferPipeline = Pipeline::createGbufferPipeline(m_context.get(), m_gbufferRenderPass.get(), {m_globalLayout.get(), m_objectMaterialLayout.get(), m_bindlessLayout.get()});

	printAllResources();


	// gui renderer
	m_guiRenderer = GuiRenderer::createGuiRenderer(m_context.get(), window, m_imguiRenderPass.get(), m_swapChain.get());
	m_guiRenderer->createViewPortDescriptorSet({m_gbufferAttachments[0].albedo.get(), m_gbufferAttachments[1].albedo.get()});


	scene();

	//descriptor set update

	std::vector<Material> materials;
	for (int32_t i = 0; i < m_materialList.size(); i++) {
		materials.push_back(m_materialList[i]);
	}
	m_materialBuffers[0]->updateStorageBuffer(&materials[0], sizeof(Material) * materials.size());
	m_materialBuffers[1]->updateStorageBuffer(&materials[0], sizeof(Material) * materials.size());



}

void Renderer::update() {

}

void Renderer::render() {
	vkWaitForFences(m_context->getDevice(), 1, &m_syncObjects->getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_context->getDevice(), m_swapChain->getSwapChain(), UINT64_MAX, 
		m_syncObjects->getImageAvailableSemaphores()[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		std::cout << "Swapchain out of date!" << std::endl;
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	ImVec2 newExtent = m_guiRenderer->getViewportSize();
	if (abs((int)newExtent.x - (int)m_extent.width) >= 1 ||
		abs((int)newExtent.y - (int)m_extent.height) >= 1) {
		recreateViewport(newExtent);
	}

	vkResetFences(m_context->getDevice(), 1, &m_syncObjects->getInFlightFences()[currentFrame]);

	vkResetCommandBuffer(m_commandBuffers->getCommandBuffers()[currentFrame], 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(m_commandBuffers->getCommandBuffers()[currentFrame], &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	// record command buffer
	recordGbufferCommandBuffer();

	transferImageLayout(m_commandBuffers->getCommandBuffers()[currentFrame], m_gbufferAttachments[currentFrame].albedo.get(), 
	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
	VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, 
	VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
	
	
	recordImGuiCommandBuffer(imageIndex);


	transferImageLayout(m_commandBuffers->getCommandBuffers()[currentFrame], m_gbufferAttachments[currentFrame].albedo.get(), 
	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 
	VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, 
	VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	// end

	if (vkEndCommandBuffer(m_commandBuffers->getCommandBuffers()[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_syncObjects->getImageAvailableSemaphores()[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers->getCommandBuffers()[currentFrame];

	VkSemaphore signalSemaphores[] = { m_syncObjects->getRenderFinishedSemaphores()[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_context->getGraphicsQueue(), 1, &submitInfo, m_syncObjects->getInFlightFences()[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_swapChain->getSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(m_context->getPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreateSwapChain();
		std::cout << "Swapchain out of date!" << std::endl;
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::recreateSwapChain() {
	vkDeviceWaitIdle(m_context->getDevice());

	int32_t width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	m_imguiFrameBuffers.clear();
	// m_gbufferFrameBuffers.clear();
	// m_gbufferAttachments.clear();
	m_swapChain.reset();
	m_swapChain = SwapChain::createSwapChain(window, m_context.get());
	// m_extent = m_swapChain->getSwapChainExtent();

	// m_gbufferAttachments.resize(MAX_FRAMES_IN_FLIGHT);
	// for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
	// 	m_gbufferAttachments[i].albedo = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	// 	m_gbufferAttachments[i].normal = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	// 	m_gbufferAttachments[i].position = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	// 	m_gbufferAttachments[i].pbr = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	// 	m_gbufferAttachments[i].depth = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	// }

	// m_gbufferFrameBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	// for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
	// 	m_gbufferFrameBuffers[i] = FrameBuffer::createGbufferFrameBuffer(m_context.get(), m_gbufferRenderPass.get(), m_gbufferAttachments[i], m_extent);
	// }

	m_imguiFrameBuffers.resize(m_swapChain->getSwapChainImages().size());
	for (int i = 0; i < m_swapChain->getSwapChainImages().size(); i++) {
		m_imguiFrameBuffers[i] = FrameBuffer::createImGuiFrameBuffer(m_context.get(), m_imguiRenderPass.get(), m_swapChain->getSwapChainImageViews()[i], m_swapChain->getSwapChainExtent());

	}

	// m_guiRenderer->createViewPortDescriptorSet({m_gbufferAttachments[0].albedo.get(), m_gbufferAttachments[1].albedo.get()});
}

void Renderer::recreateViewport(ImVec2 newExtent) {
	vkDeviceWaitIdle(m_context->getDevice());

	std::cout << "newExtent: " << newExtent.x << " " << newExtent.y << std::endl;
	std::cout << "m_extent: " << m_extent.width << " " << m_extent.height << std::endl;

	if (newExtent.x <= 0 || newExtent.y <= 0) {
		return;
	}

	m_extent.width = static_cast<uint32_t>(newExtent.x);
	m_extent.height = static_cast<uint32_t>(newExtent.y);
	m_gbufferFrameBuffers.clear();
	m_gbufferAttachments.clear();

	m_gbufferAttachments.resize(MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_gbufferAttachments[i].albedo = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].normal = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].position = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].pbr = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].depth = Texture::createAttachmentTexture(m_context.get(), m_extent.width, m_extent.height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	}
	
	m_gbufferFrameBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_gbufferFrameBuffers[i] = FrameBuffer::createGbufferFrameBuffer(m_context.get(), m_gbufferRenderPass.get(), m_gbufferAttachments[i], m_extent);
	}

	m_guiRenderer->createViewPortDescriptorSet({m_gbufferAttachments[0].albedo.get(), m_gbufferAttachments[1].albedo.get()});
}

void Renderer::createDefaultModels()
{
	glm::vec4 defaultColor = glm::vec4(1.0f);

	auto defaultTex = Texture::createDefaultTexture(m_context.get(), defaultColor);
	int32_t texIndex = static_cast<int32_t>(m_textureList.size());
	m_textureList.push_back(std::move(defaultTex));

	Material defaultMat{};
	defaultMat.albedoTexIndex = texIndex;
	defaultMat.baseColor = defaultColor;
	int32_t matIndex = static_cast<int32_t>(m_materialList.size());
	m_materialList.push_back(defaultMat);

	auto createAndRegisterMesh = [&](std::unique_ptr<Mesh> mesh) -> int32_t {
		int32_t meshIndex = static_cast<int32_t>(m_meshList.size());
		m_meshList.push_back(std::move(mesh));
		return meshIndex;
	};

	// Plane
	{
		Model model;
		model.mesh.push_back(createAndRegisterMesh(Mesh::createPlaneMesh(m_context.get())));
		model.material.push_back(matIndex);
		m_modelList.push_back(model);
	}

	// Box
	{
		Model model;
		model.mesh.push_back(createAndRegisterMesh(Mesh::createBoxMesh(m_context.get())));
		model.material.push_back(matIndex);
		m_modelList.push_back(model);
	}

	// Sphere
	{
		Model model;
		model.mesh.push_back(createAndRegisterMesh(Mesh::createSphereMesh(m_context.get())));
		model.material.push_back(matIndex);
		m_modelList.push_back(model);
	}

	std::cout << "[Renderer] Default Models Loaded: Plane, Box, Sphere" << std::endl;
}

void Renderer::loadModel(const std::string& modelPath) {
	Model model = ModelLoader::loadGLTFModel(modelPath, m_context.get(), m_meshList, m_textureList, m_materialList);
	m_modelList.push_back(model);
}


void Renderer::printAllResources() {
	std::cout << "======== [Renderer Resources] ========" << std::endl;

	std::cout << "\n[Meshes] Count: " << m_meshList.size() << std::endl;
	for (size_t i = 0; i < m_meshList.size(); ++i)
		std::cout << " * Mesh[" << i << "] pointer: " << m_meshList[i].get() << std::endl;

	std::cout << "\n[Textures] Count: " << m_textureList.size() << std::endl;
	for (size_t i = 0; i < m_textureList.size(); ++i)
		std::cout << " * Texture[" << i << "] pointer: " << m_textureList[i].get() << std::endl;

	std::cout << "\n[Materials] Count: " << m_materialList.size() << std::endl;
	for (size_t i = 0; i < m_materialList.size(); ++i) {
		std::cout << " * Material[" << i << "]" << std::endl;
		printMaterial(m_materialList[i]);
	}

	std::cout << "\n[Models] Count: " << m_modelList.size() << std::endl;
	for (size_t i = 0; i < m_modelList.size(); ++i) {
		std::cout << " * Model[" << i << "]" << std::endl;
		printModel(m_modelList[i]);
	}

	std::cout << "=======================================" << std::endl;
}


void Renderer::recordGbufferCommandBuffer() {
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_gbufferRenderPass->getRenderPass();
	renderPassInfo.framebuffer = m_gbufferFrameBuffers[currentFrame]->getFrameBuffer();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_extent;

	std::array<VkClearValue, 5> clearValues{};
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[2].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[3].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[4].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();
	
	vkCmdBeginRenderPass(m_commandBuffers->getCommandBuffers()[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	vkCmdBindPipeline(m_commandBuffers->getCommandBuffers()[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_gbufferPipeline->getPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_extent.width);
	viewport.height = static_cast<float>(m_extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_commandBuffers->getCommandBuffers()[currentFrame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_extent;
	vkCmdSetScissor(m_commandBuffers->getCommandBuffers()[currentFrame], 0, 1, &scissor);

	// camera buffer update
	CameraBuffer cameraBuffer{};
	cameraBuffer.view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
	cameraBuffer.proj = glm::perspective(glm::radians(45.0f), (float)m_extent.width / (float)m_extent.height, 0.1f, 100.0f);
	cameraBuffer.proj[1][1] *= -1; // flip Y axis
	cameraBuffer.camPos = m_cameraPos;
	m_cameraBuffers[currentFrame]->updateUniformBuffer(&cameraBuffer, sizeof(CameraBuffer));

	// bind descriptor sets
	vkCmdBindDescriptorSets(m_commandBuffers->getCommandBuffers()[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_gbufferPipeline->getPipelineLayout(), 0, 1, &m_globlaDescSets[currentFrame]->getDescriptorSet(), 0, nullptr);
	vkCmdBindDescriptorSets(m_commandBuffers->getCommandBuffers()[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_gbufferPipeline->getPipelineLayout(), 1, 1, &m_objectMaterialDescSets[currentFrame]->getDescriptorSet(), 0, nullptr);
	vkCmdBindDescriptorSets(m_commandBuffers->getCommandBuffers()[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_gbufferPipeline->getPipelineLayout(), 2, 1, &m_bindlessDescSets[currentFrame]->getDescriptorSet(), 0, nullptr);
	
	std::unordered_map<int32_t, std::vector<int32_t>> objectMap;
	std::vector<ModelBuffer> modelBuffers(m_objects.size());
	for (int32_t i = 0; i < m_objects.size(); i++) {
		objectMap[m_objects[i].modelIndex].push_back(i);
		modelBuffers[i].model = m_objects[i].transform;
	}
	m_modelBuffers[currentFrame]->updateStorageBuffer(&modelBuffers[0], sizeof(ModelBuffer) * m_objects.size());

	std::vector<ObjectInstance> objectInstances;
	objectInstances.reserve(m_objects.size() * 16);
	int32_t index = 0;
	for (const auto& [key, value] : objectMap) {
		for (int32_t i = 0; i < m_modelList[key].mesh.size(); i++) {
			int32_t startIndex = index;
			for (int32_t j = 0; j < value.size(); j++) {
				int32_t materialIndex;
				if (m_objects[value[j]].overrideMaterialIndex.size() > i) {
					materialIndex = m_objects[value[j]].overrideMaterialIndex[i];
				}
				else {
					materialIndex = m_modelList[key].material[i];
				}
				objectInstances.push_back({value[j], materialIndex});
				index++;
			}
			m_meshList[m_modelList[key].mesh[i]]->drawInstance(m_commandBuffers->getCommandBuffers()[currentFrame], value.size(), startIndex);
		}
	}
	m_objectInstanceBuffers[currentFrame]->updateStorageBuffer(&objectInstances[0], sizeof(ObjectInstance) * objectInstances.size());
	
	vkCmdEndRenderPass(m_commandBuffers->getCommandBuffers()[currentFrame]);
}

void Renderer::recordImGuiCommandBuffer(uint32_t imageIndex) {
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_imguiRenderPass->getRenderPass();
	renderPassInfo.framebuffer = m_imguiFrameBuffers[imageIndex]->getFrameBuffer();
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = m_swapChain->getSwapChainExtent();

	std::array<VkClearValue, 1> clearValues{};
	clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(m_commandBuffers->getCommandBuffers()[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	m_guiRenderer->newFrame();
	m_guiRenderer->render(currentFrame, m_commandBuffers->getCommandBuffers()[currentFrame]);
	vkCmdEndRenderPass(m_commandBuffers->getCommandBuffers()[currentFrame]);
}


void Renderer::scene() {
	m_objects.clear();

	// cam
	glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 5.0f);
	glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.1f, 1.1f, 0.0f));
		obj.overrideMaterialIndex.push_back(0);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.1f, 0.0f));
		obj.overrideMaterialIndex.push_back(1);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.1f, 1.1f, 0.0f));
		obj.overrideMaterialIndex.push_back(2);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.1f, 0.0f, 0.0f));
		obj.overrideMaterialIndex.push_back(3);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
		obj.overrideMaterialIndex.push_back(4);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.1f, 0.0f, 0.0f));
		obj.overrideMaterialIndex.push_back(5);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.1f, -1.1f, 0.0f));
		obj.overrideMaterialIndex.push_back(6);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.1f, 0.0f));
		obj.overrideMaterialIndex.push_back(7);
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(1.1f, -1.1f, 0.0f));
		obj.overrideMaterialIndex.push_back(8);
		m_objects.push_back(obj);
	}
}

void Renderer::transferImageLayout( VkCommandBuffer cmd, Texture* texture, VkImageLayout oldLayout, VkImageLayout newLayout,
	VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage) {
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcAccessMask = srcAccessMask;
	barrier.dstAccessMask = dstAccessMask;
	barrier.image = texture->getImageBuffer()->getImage();
	if (texture->getFormat() == VK_FORMAT_D32_SFLOAT) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		cmd,
		srcStage,
		dstStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
}
