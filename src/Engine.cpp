
#include <iostream>

#define IMGUI_USER_CONFIG "my_imgui_config.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl2.h"
#include "imgui_freetype.h"
#include <stdio.h>
#include <SDL.h>
#include <SDL_opengl.h>
#ifdef _WIN32
#include <windows.h>        // SetProcessDPIAware()
#endif

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include "Engine.h"

void Engine::run(std::unique_ptr<App> app) {

    this->app = std::move(app);
    this->app->onInit();
    mainLoop();
	this->app->onCleanup();

}

int Engine::initialize() {

    plog::init(plog::debug, "app.log");

	PLOG_INFO << "Initializing engine";
	
#ifdef _WIN32
    ::SetProcessDPIAware();
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
		PLOG_ERROR << "SDL_Init() failed: " << SDL_GetError();
        return -1;
    }

#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    float main_scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)(width * main_scale), (int)(height * main_scale), window_flags);
    if (window == nullptr)
    {
		PLOG_ERROR << "SDL_CreateWindow() failed: " << SDL_GetError();
        return -1;
    }

    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL2_Init();

    style.FontSizeBase = 20.0f;

	const char* font_path = "AporeticSerifMonoNerdFont.ttf";
    ImFont* font = io.Fonts->AddFontFromFileTTF(font_path);
    if (font == nullptr) {
        PLOG_ERROR << "Failed to load font: " << font_path;
        return -1;
	}

    //ImFontConfig lucide_icons_config;
    //lucide_icons_config.MergeMode = true;
    //lucide_icons_config.PixelSnapH = true;
    //io.Fonts->AddFontFromFileTTF("lucide.ttf", 0.0f, &lucide_icons_config);

    return 0;

}

void Engine::cleanup() {

    PLOG_INFO << "Cleaning up engine";

    // Cleanup
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

	PLOG_INFO << "Engine cleanup complete";

}

void Engine::mainLoop() {

	PLOG_INFO << "Starting main loop";

	ImGuiIO& io = ImGui::GetIO();

	while (is_running) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                is_running = false;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                is_running = false;
        }

        if (!is_running)
            break;

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        app->onUpdate(io.DeltaTime);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        app->onRender();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);

		// break; // For now, we only run one frame in this example. Remove this line to run continuously.

	}

    PLOG_INFO << "Exiting main loop";

}
