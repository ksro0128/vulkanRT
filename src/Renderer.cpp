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
}

void Renderer::init(GLFWwindow* window) {
	std::cout << "Renderer::init" << std::endl;
	this->window = window;
	m_context = VulkanContext::createVulkanContext(window);
	m_swapChain = SwapChain::createSwapChain(window, m_context.get());
	m_syncObjects = SyncObjects::createSyncObjects(m_context.get());
	createDefaultModels();

	std::string modelPath = "./assets/models/lion_head_1k.gltf/lion_head_1k.gltf";
	loadModel(modelPath);

	m_globalLayout = DescriptorSetLayout::createGlobalDescriptorSetLayout(m_context.get());
	m_objectMaterialLayout = DescriptorSetLayout::createObjectMaterialDescriptorSetLayout(m_context.get());
	m_textureLayout = DescriptorSetLayout::createTextureDescriptorSetLayout(m_context.get());

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_cameraBuffers[i] = UniformBuffer::createUniformBuffer(m_context.get(), sizeof(CameraBuffer));
		m_lightBuffers[i] = StorageBuffer::createStorageBuffer(m_context.get(), sizeof(LightBuffer));
		m_modelBuffers[i] = StorageBuffer::createStorageBuffer(m_context.get(), sizeof(ModelBuffer) * MAX_OBJECT_COUNT);
		m_materialBuffers[i] = StorageBuffer::createStorageBuffer(m_context.get(), sizeof(Material) * MAX_MATERIAL_COUNT);
	}

	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		m_globlaDescSets[i] = DescriptorSet::createGlobalDescriptorSet(m_context.get(), m_globalLayout.get(), m_cameraBuffers[i].get(), m_lightBuffers[i].get());
		m_objectMaterialDescSets[i] = DescriptorSet::createObjectMaterialDescriptorSet(m_context.get(), m_objectMaterialLayout.get(), m_modelBuffers[i].get(), m_materialBuffers[i].get());
		m_textureDescSets[i] = DescriptorSet::createTextureDescriptorSet(m_context.get(), m_textureLayout.get(), m_textureList);
	}

	printAllResources();

}

void Renderer::update() {

}

void Renderer::render() {

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
