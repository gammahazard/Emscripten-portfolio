#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <emscripten.h>

// =================================================
// 1. JAVASCRIPT BRIDGE
// =================================================

// Redirects to the Doom folder
EM_JS(void, redirect_to_doom, (), {
    console.log("Attempting to launch Doom...");
    window.open('doom/', '_blank');
});

// Helper to make the HTML body black (removes white borders)
EM_JS(void, set_body_black, (), {
    document.body.style.backgroundColor = "#000000";
    document.body.style.margin = "0";
    document.body.style.overflow = "hidden";
    // Force canvas to focus
    document.getElementById('canvas').focus(); 
});

// =================================================
// 2. RENDER LOOP
// =================================================

SDL_Window* window;
SDL_GLContext gl_context;

void main_loop() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {}
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Setup a full-screen window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Launcher", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

    // CENTER THE BUTTON
    ImVec2 window_size = ImGui::GetIO().DisplaySize;
    float button_width = 300.0f;
    float button_height = 100.0f;
    
    ImGui::SetCursorPosX((window_size.x - button_width) * 0.5f);
    ImGui::SetCursorPosY((window_size.y - button_height) * 0.5f);

    // The Style
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.0f, 0.0f, 1.0f)); // Doom Red
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    // The Interaction
    if (ImGui::Button("LAUNCH DOOM", ImVec2(button_width, button_height))) {
        redirect_to_doom();
    }
    
    ImGui::PopStyleColor(3);
    ImGui::End();

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(window, gl_context);
    glClearColor(0, 0, 0, 1); // Black background
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

// =================================================
// 3. ENTRY POINT
// =================================================
int main(int, char**) {
    // SDL Setup
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return -1;
    
    // WebGL 2 Context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    window = SDL_CreateWindow("Launcher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    gl_context = SDL_GL_CreateContext(window);

    // ImGui Setup
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    
    // Set Font Scale for visibility
    ImGui::GetIO().FontGlobalScale = 2.0f;

    set_body_black();
    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}