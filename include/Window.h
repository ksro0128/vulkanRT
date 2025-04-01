#pragma once

#include "Common.h"

class Window {
public:
	static std::unique_ptr<Window> createWindow();
	~Window();


	GLFWwindow* getWindow() { return window; }

private:
	Window() {}

	GLFWwindow* window;

	void initWindow();
	void cleanup();
};