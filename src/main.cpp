#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <emscripten.h>

// Global variables to manage state
SDL_Window* window;
SDL_GLContext gl_context;

// This function runs every single frame (like a game loop)
void main_loop() {
    // 1. Process Input (Keyboard/Mouse)
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) { /* Handle quit if needed */ }
    }

    // 2. Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // 3. Define Your UI Here! ============================
    ImGui::Begin("My Portfolio"); 
    ImGui::Text("Welcome to my C++ WebAssembly App.");
    ImGui::Text("This is running directly in your browser.");
    
    static int clicks = 0;
    if (ImGui::Button("Click Me")) {
        clicks++;
    }
    ImGui::Text("Button clicked: %d times", clicks);
    ImGui::End();
    // ====================================================

    // 4. Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(window, gl_context);
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Background color
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

int main(int, char**) {
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        return -1;
    }

    // Setup WebGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create Window
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("Wasm Portfolio", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    gl_context = SDL_GL_CreateContext(window);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    // Start the endless loop
    // 0 means "Run as fast as possible" (usually 60fps)
    // 1 means "Simulate infinite loop" (required for browser)
    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}