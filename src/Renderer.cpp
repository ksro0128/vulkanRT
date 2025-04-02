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

	
	for (int i = 0; i < 1; i++)
		tmpTexture[i] = Texture::createTexture(m_context.get(), "./assets/textures/brick_wall_13_4k.gltf/textures/brick_wall_13_diff_4k.jpg");
	std::cout << "done" << std::endl;

	//Model::createModel(m_context.get(), m_mapSets, "./assets/textures/brick_wall_13_4k.gltf/textures/brick_wall_13_diff_4k.jpg");
}

void Renderer::update() {

}

void Renderer::render() {

}