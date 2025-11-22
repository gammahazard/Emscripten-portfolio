#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <emscripten.h>

// =================================================
// 1. JAVASCRIPT BRIDGE
// =================================================

// Redirects to the Doom folder (Mobile Friendly Version)
EM_JS(void, redirect_to_doom, (), {
    console.log("Navigating to Doom...");
    // FIX: We use window.location.href instead of window.open.
    // Browsers (especially Mobile Safari/Chrome) block window.open 
    // if it's inside an async call or not a direct "click" event.
    // Navigating the CURRENT tab is always allowed.
    window.location.href = 'doom/';
});

// Helper to make the HTML body black (removes white borders)
EM_JS(void, set_body_black, (), {
    document.body.style.backgroundColor = "#000000";
    document.body.style.margin = "0";
    document.body.style.overflow = "hidden";
    // Force canvas to focus so keyboard inputs work immediately
    var canvas = document.getElementById('canvas');
    if (canvas) canvas.focus(); 
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
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize);
    
    // No title bar, no resize handle, no background (we use the OpenGL clear color)
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | 
                             ImGuiWindowFlags_NoResize | 
                             ImGuiWindowFlags_NoMove | 
                             ImGuiWindowFlags_NoBackground;

    ImGui::Begin("Launcher", NULL, flags);

    // --- LAYOUT LOGIC ---
    float window_width = io.DisplaySize.x;
    float window_height = io.DisplaySize.y;

    // 1. The Text
    const char* text_line_1 = "This is my Emscripten portfolio.";
    const char* text_line_2 = "For now, enjoy Doom while I figure out how to expand it.";
    
    // Calculate centering for text
    float text_w1 = ImGui::CalcTextSize(text_line_1).x;
    float text_w2 = ImGui::CalcTextSize(text_line_2).x;
    
    // Position text block slightly above center
    float content_start_y = window_height * 0.40f;
    
    ImGui::SetCursorPosY(content_start_y);
    
    // Render Line 1 (Centered)
    ImGui::SetCursorPosX((window_width - text_w1) * 0.5f);
    ImGui::Text("%s", text_line_1);
    
    // Render Line 2 (Centered)
    ImGui::SetCursorPosX((window_width - text_w2) * 0.5f);
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", text_line_2); // Grey color

    // 2. The Button
    float button_width = 300.0f;
    float button_height = 80.0f;
    
    // Add some spacing between text and button
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Center the button horizontally
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);

    // Style the button (Doom Red)
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.0f, 0.0f, 1.0f)); 
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
    
    if (ImGui::Button("LAUNCH DOOM", ImVec2(button_width, button_height))) {
        redirect_to_doom();
    }
    
    ImGui::PopStyleColor(3);
    ImGui::End();

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(window, gl_context);
    
    // Clear background to black
    glClearColor(0, 0, 0, 1); 
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
    
    // WebGL 2 Context settings
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window
    window = SDL_CreateWindow("Launcher", 
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              1280, 720, 
                              SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    gl_context = SDL_GL_CreateContext(window);

    // ImGui Setup
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    
    // Increase Font Size for Visibility
    ImGui::GetIO().FontGlobalScale = 2.0f;

    // Initial HTML setup
    set_body_black();
    
    // Start Loop
    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}