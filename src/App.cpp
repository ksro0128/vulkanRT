#include "include/App.h"

App::App() {
	init();
}

App::~App() {
	cleanup();
}

void App::run() {
	std::cout << "App::run" << std::endl;

	while (!glfwWindowShouldClose(window->getWindow())) {
		glfwPollEvents();
	}
}

void App::init() {
	std::cout << "App::init" << std::endl;
	window = Window::createWindow();
	renderer = Renderer::createRenderer(window->getWindow());
}

void App::cleanup()
{
	std::cout << "App::cleanup" << std::endl;
}