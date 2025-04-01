#include "include/App.h"

App::App() {
	init();
}

App::~App() {
	cleanup();
}

void App::run() {
	std::cout << "App::run" << std::endl;

	while (!glfwWindowShouldClose(m_window->getWindow())) {
		glfwPollEvents();
	}
}

void App::init() {
	std::cout << "App::init" << std::endl;
	m_window = Window::createWindow();
	m_renderer = Renderer::createRenderer(m_window->getWindow());
}

void App::cleanup()
{
	std::cout << "App::cleanup" << std::endl;
}