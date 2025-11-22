#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <emscripten.h>

// global variables
SDL_Window* window;
SDL_GLContext gl_context;

// theme state
bool is_dark_mode = true;

// global click count variable
int clicks = 0; 

// em_js macros for browser interaction

// saves click count to localstorage
EM_JS(void, save_to_local_storage, (int count), {
    localStorage.setItem('click_count', count.toString());
});

// loads click count from localstorage
EM_JS(int, load_from_local_storage, (), {
    var count = localStorage.getItem('click_count');
    // return 0 if no item exists
    return count ? parseInt(count) : 0;
});

// clears the click count
EM_JS(void, clear_local_storage, (), {
    localStorage.removeItem('click_count');
});

// sets html background color and removes scrollbars
EM_JS(void, set_html_body_color, (float r, float g, float b), {
    // convert float colors to css rgb(0-255)
    var r_int = Math.round(r * 255);
    var g_int = Math.round(g * 255);
    var b_int = Math.round(b * 255);
    var color_string = 'rgb(' + r_int + ',' + g_int + ',' + b_int + ')';
    
    // scrollbar and body fixes
    document.documentElement.style.overflow = 'hidden'; 
    document.body.style.backgroundColor = color_string;
    document.body.style.margin = '0';
    document.body.style.overflow = 'hidden'; 
    
    var canvas = document.getElementById('canvas'); 
    if (canvas) {
        canvas.style.backgroundColor = color_string;
        canvas.style.width = '100vw';
        canvas.style.height = '100vh';
        canvas.style.display = 'block'; 
    }
});

// redirects the browser to the new doom web port
EM_JS(void, redirect_to_doom, (), {
    // *** MODIFIED LINE: window.open opens link in a new tab ***
    window.open('http://wasm.continuation-labs.com/d3demo/', '_blank'); 
});


// applies custom styling
void SetCustomStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    // rounding and curves
    style.FrameRounding = 5.0f; 
    style.GrabRounding = 5.0f;
    style.WindowRounding = 8.0f; 
    style.PopupRounding = 5.0f;
    style.ChildRounding = 5.0f;

    // spacing and padding
    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(10, 8);
    style.ItemSpacing = ImVec2(10, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.ScrollbarSize = 12.0f;

    // custom color blending
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f); 
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.55f, 1.00f); 
}

// switches the ui theme
void SetTheme(bool dark) {
    ImGuiStyle& style = ImGui::GetStyle();

    if (dark) {
        ImGui::StyleColorsDark();
        // ensure white text in dark mode
        style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        
        // dark theme tweak: frame color
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.54f);
        is_dark_mode = true;
    } else {
        ImGui::StyleColorsLight();
        // light theme tweak: text color
        style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        is_dark_mode = false;
    }
    
    // synchronize html background
    ImVec4 bg_color = style.Colors[ImGuiCol_WindowBg];
    set_html_body_color(bg_color.x, bg_color.y, bg_color.z);
}


// runs every frame (game loop)
void main_loop() {
    // 1. handle input
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT) { /* handle quit if needed */ }
    }

    // 2. start imgui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // 3. ui definition
    
    // full-screen window setup
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(io.DisplaySize); 
    
    // flags to remove window decorations
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    
    ImGui::Begin("wasm portfolio showcase", NULL, flags); 
    
    ImGui::Text("welcome to my c++ webassembly app.");
    ImGui::Separator();
    
    // theme toggle
    const char* theme_button_text = is_dark_mode ? "switch to light mode" : "switch to dark mode";
    
    if (ImGui::Button(theme_button_text)) {
        is_dark_mode = !is_dark_mode;
        SetTheme(is_dark_mode);
    }
    
    // layout fix: ensure space and separation for the doom button
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // doom button
    if (ImGui::Button("run classic doom (wasm demo)")) {
        redirect_to_doom();
    }
    
    // layout fix: separate doom button from counter
    ImGui::Spacing();

    // app content (click counter)
    if (ImGui::Button("click me (c++ logic)")) {
        clicks++;
        save_to_local_storage(clicks);
    }
    ImGui::SameLine();
    ImGui::Text("button clicked: %d times", clicks);
    
    // reset button (optional)
    ImGui::SameLine();
    if (ImGui::Button("reset count")) {
        clicks = 0;
        clear_local_storage();
    }

    ImGui::End();
    // ====================================================

    // 4. rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(window, gl_context);
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4 window_bg = style.Colors[ImGuiCol_WindowBg];
    
    glClearColor(window_bg.x, window_bg.y, window_bg.z, window_bg.w);
    
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

int main(int, char**) {
    // setup sdl 
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        return -1;
    }

    // setup webgl attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // create window
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("wasm portfolio", 
                              SDL_WINDOWPOS_CENTERED, 
                              SDL_WINDOWPOS_CENTERED, 
                              1920, 
                              1080, 
                              window_flags);
    gl_context = SDL_GL_CreateContext(window);

    // setup dear imgui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    // disable unneeded imgui io flags
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableGamepad; 
    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;

    // load initial click count
    clicks = load_from_local_storage();

    SetCustomStyle();   
    SetTheme(true);     

    // setup platform/renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 300 es");

    // start the endless loop
    emscripten_set_main_loop(main_loop, 0, 1);

    return 0;
}