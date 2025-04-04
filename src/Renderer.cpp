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

	m_extent = m_swapChain->getSwapChainExtent();

	createDefaultModels();

	std::string modelPath = "./assets/models/lion_head_1k.gltf/lion_head_1k.gltf";
	loadModel(modelPath);

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
		m_materialBuffers[i] = StorageBuffer::createStorageBuffer(m_context.get(), sizeof(Material) * MAX_OBJECT_COUNT * 2);
	}

	// descriptorset
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_globlaDescSets[i] = DescriptorSet::createGlobalDescriptorSet(m_context.get(), m_globalLayout.get(), m_cameraBuffers[i].get(), m_lightBuffers[i].get());
		m_objectMaterialDescSets[i] = DescriptorSet::createObjectMaterialDescriptorSet(m_context.get(), m_objectMaterialLayout.get(), m_objectInstanceBuffers[i].get());
		m_bindlessDescSets[i] = DescriptorSet::createBindlessDescriptorSet(m_context.get(), m_bindlessLayout.get(), m_modelBuffers[i].get(), m_materialBuffers[i].get(), m_textureList);
	}

	//renderpass
	m_gbufferRenderPass = RenderPass::createGbufferRenderPass(m_context.get());

	// attachment
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_gbufferAttachments[i].albedo = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].normal = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].position = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].pbr = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		m_gbufferAttachments[i].depth = Texture::createAttachmentTexture(m_context.get(), m_swapChain->getSwapChainExtent().width, m_swapChain->getSwapChainExtent().height, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	// framebuffer
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_gbufferFrameBuffers[i] = FrameBuffer::createGbufferFrameBuffer(m_context.get(), m_gbufferRenderPass.get(), m_gbufferAttachments[i], m_extent);
	}

	// pipeline
	m_gbufferPipeline = Pipeline::createGbufferPipeline(m_context.get(), m_gbufferRenderPass.get(), {m_globalLayout.get(), m_objectMaterialLayout.get(), m_bindlessLayout.get()});

	printAllResources();


	scene();

	//descriptor set update

}

void Renderer::update() {

}

void Renderer::render() {
	//vkWaitForFences(m_context->getDevice(), 1, &m_syncObjects->getInFlightFences()[currentFrame], VK_TRUE, UINT64_MAX);

	//uint32_t imageIndex;
	//VkResult result = vkAcquireNextImageKHR(m_context->getDevice(), m_swapChain->getSwapChain(), UINT64_MAX, 
	//	m_syncObjects->getImageAvailableSemaphores()[currentFrame], VK_NULL_HANDLE, &imageIndex);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR) {
	//	std::cout << "Swapchain out of date!" << std::endl;
	//	// recreateSwapChain();
	//	return;
	//}
	//else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
	//	throw std::runtime_error("failed to acquire swap chain image!");
	//}

	//vkResetFences(m_context->getDevice(), 1, &m_syncObjects->getInFlightFences()[currentFrame]);

	//vkResetCommandBuffer(m_commandBuffers->getCommandBuffers()[currentFrame], 0);

	//VkCommandBufferBeginInfo beginInfo{};
	//beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	//if (vkBeginCommandBuffer(m_commandBuffers->getCommandBuffers()[currentFrame], &beginInfo) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to begin recording command buffer!");
	//}

	//// record command buffer
	//recordGbufferCommandBuffer();

	//// end

	//if (vkEndCommandBuffer(m_commandBuffers->getCommandBuffers()[currentFrame]) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to record command buffer!");
	//}

	//VkSubmitInfo submitInfo{};
	//submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	//VkSemaphore waitSemaphores[] = { m_syncObjects->getImageAvailableSemaphores()[currentFrame] };
	//VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	//submitInfo.waitSemaphoreCount = 1;
	//submitInfo.pWaitSemaphores = waitSemaphores;
	//submitInfo.pWaitDstStageMask = waitStages;

	//submitInfo.commandBufferCount = 1;
	//submitInfo.pCommandBuffers = &m_commandBuffers->getCommandBuffers()[currentFrame];

	//VkSemaphore signalSemaphores[] = { m_syncObjects->getRenderFinishedSemaphores()[currentFrame] };
	//submitInfo.signalSemaphoreCount = 1;
	//submitInfo.pSignalSemaphores = signalSemaphores;

	//if (vkQueueSubmit(m_context->getGraphicsQueue(), 1, &submitInfo, m_syncObjects->getInFlightFences()[currentFrame]) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to submit draw command buffer!");
	//}

	//VkPresentInfoKHR presentInfo{};
	//presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	//presentInfo.waitSemaphoreCount = 1;
	//presentInfo.pWaitSemaphores = signalSemaphores;

	//VkSwapchainKHR swapChains[] = { m_swapChain->getSwapChain() };
	//presentInfo.swapchainCount = 1;
	//presentInfo.pSwapchains = swapChains;
	//presentInfo.pImageIndices = &imageIndex;

	//result = vkQueuePresentKHR(m_context->getPresentQueue(), &presentInfo);

	//if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
	//	// recreateSwapChain();
	//	std::cout << "Swapchain out of date!" << std::endl;
	//}
	//else if (result != VK_SUCCESS) {
	//	throw std::runtime_error("failed to present swap chain image!");
	//}

	//currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
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
	renderPassInfo.renderArea.extent = m_swapChain->getSwapChainExtent();

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
	viewport.width = static_cast<float>(m_swapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(m_swapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(m_commandBuffers->getCommandBuffers()[currentFrame], 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChain->getSwapChainExtent();
	vkCmdSetScissor(m_commandBuffers->getCommandBuffers()[currentFrame], 0, 1, &scissor);

	// camera buffer update
	CameraBuffer cameraBuffer{};
	cameraBuffer.view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, glm::vec3(0.0f, 1.0f, 0.0f));
	cameraBuffer.proj = glm::perspective(glm::radians(45.0f), (float)m_swapChain->getSwapChainExtent().width / (float)m_swapChain->getSwapChainExtent().height, 0.1f, 100.0f);
	cameraBuffer.proj[1][1] *= -1; // flip Y axis
	cameraBuffer.camPos = m_cameraPos;
	m_cameraBuffers[currentFrame]->updateUniformBuffer(&cameraBuffer, sizeof(CameraBuffer));

	// bind descriptor sets
	vkCmdBindDescriptorSets(m_commandBuffers->getCommandBuffers()[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_gbufferPipeline->getPipelineLayout(), 0, 1, &m_globlaDescSets[currentFrame]->getDescriptorSet(), 0, nullptr);
	vkCmdBindDescriptorSets(m_commandBuffers->getCommandBuffers()[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_gbufferPipeline->getPipelineLayout(), 1, 1, &m_objectMaterialDescSets[currentFrame]->getDescriptorSet(), 0, nullptr);
	vkCmdBindDescriptorSets(m_commandBuffers->getCommandBuffers()[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_gbufferPipeline->getPipelineLayout(), 2, 1, &m_bindlessDescSets[currentFrame]->getDescriptorSet(), 0, nullptr);
	for (const auto& object : m_objects) {



	}
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
		glm::vec4 pos = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

		obj.modelIndex = 0;
		obj.transform = glm::translate(glm::mat4(1.0f), glm::vec3(pos));
		m_objects.push_back(obj);
	}
	{
		Object obj;
		obj.modelIndex = 1;
		obj.transform = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
		m_objects.push_back(obj);
	}

}