#pragma once

#include "Common.h"
#include "VulkanContext.h"
#include "SwapChain.h"
#include "SyncObjects.h"
#include "Buffer.h"
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "Material.h"

class Renderer {
public:
	static std::unique_ptr<Renderer> createRenderer(GLFWwindow* window);
	~Renderer();

	void update();
	void render();
private:
	Renderer() {}
	void cleanup();
	void init(GLFWwindow* window);

	GLFWwindow* window;
	std::unique_ptr<VulkanContext> m_context;
	std::unique_ptr<SwapChain> m_swapChain;
	std::unique_ptr<SyncObjects> m_syncObjects;
	MapSets m_mapSets;

	std::unique_ptr<ImageBuffer> tmp[30];
	std::shared_ptr<Mesh> tmpMesh[1000];
	std::unique_ptr<Texture> tmpTexture[30];


};