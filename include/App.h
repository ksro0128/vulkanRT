#pragma once

#include "Window.h"
#include "Renderer.h"

class App {
public:
	App();
	~App();
	void run();

private:
	std::unique_ptr<Window> window;
	std::unique_ptr<Renderer> renderer;

	void init();
	void cleanup();
};