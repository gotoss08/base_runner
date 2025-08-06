
#include "App.h"
#include "Engine.h"

#include <plog/Log.h>
#include <imgui.h>

ImGuiIO* io;
bool show_demo_window = true;
bool show_another_window = false;

void App::onInit() {
	PLOG_INFO << "Initializing application";

	io = &ImGui::GetIO();
}

void App::onUpdate(float deltaTime) {
	PLOG_DEBUG << "Updating application with deltaTime: " << deltaTime;
}

void App::onRender() {
	PLOG_DEBUG << "Rendering application";

	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

		ImVec4 clear_color = engine.getClearColor();

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		engine.setClearColor(clear_color);                     // Set the clear color in the engine

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
        ImGui::Text((const char*)u8"Привет мир!");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

}

void App::onCleanup() {
	PLOG_INFO << "Cleaning up application";
}
