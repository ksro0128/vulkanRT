#pragma once

#include "Common.h"
#include "VulkanContext.h"
#include "SwapChain.h"
#include "SyncObjects.h"


class Renderer {
public:
	static std::unique_ptr<Renderer> createRenderer(GLFWwindow* window);
	~Renderer();
private:
	Renderer() {}
	void cleanup();
	void init(GLFWwindow* window);

	GLFWwindow* window;
	std::unique_ptr<VulkanContext> m_context;
	std::unique_ptr<SwapChain> m_swapChain;
	std::unique_ptr<SyncObjects> m_syncObjects;


};