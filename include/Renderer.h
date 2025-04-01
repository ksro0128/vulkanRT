#pragma once

#include "Common.h"
#include "VulkanContext.h"

class Renderer {
public:
	static std::unique_ptr<Renderer> createRenderer(GLFWwindow* window);
	~Renderer();
private:
	Renderer() {}
	void cleanup();
	void init(GLFWwindow* window);

	GLFWwindow* window;
	std::unique_ptr<VulkanContext> context;

};