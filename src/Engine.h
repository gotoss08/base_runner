#pragma once

#include <string>
#include <memory>
#include <stdexcept>

#include <SDL.h>
#include <imgui.h>

#include "App.h"

class Engine {
public:
	Engine(int width, int height, std::string title) :
		width(width), height(height), title(std::move(title)) {
		if (initialize() != 0) {
			throw std::runtime_error("Failed to initialize the engine");
		}
	}
	~Engine() {
		cleanup();
	}

	void run(std::unique_ptr<App> app);

	int getWidth() const { return width; }
	int getHeight() const { return height; }
	SDL_Window* getWindow() const { return window; }
	SDL_GLContext getGLContext() const { return gl_context; }
	bool isRunning() const { return is_running; }
	void setRunning(bool running) { is_running = running; }
	ImVec4 getClearColor() const { return clear_color; }
	void setClearColor(const ImVec4& color) { clear_color = color; }
private:
	int width;
	int height;
	std::string title;

	std::unique_ptr<App> app;
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context = nullptr;

	bool is_running = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	int initialize();
	void mainLoop();
	void cleanup();
};
