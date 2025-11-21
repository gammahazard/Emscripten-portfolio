cmake_minimum_required(VERSION 3.10)
project(WasmPortfolio)

set(CMAKE_CXX_STANDARD 17)

# Output all build artifacts to a "dist" folder in the root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)

# Emscripten Flags: Use SDL2, WebGL, and allow memory growth
set(EMSCRIPTEN_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${EMSCRIPTEN_FLAGS}")

# Define ImGui Sources
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/libs/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(index src/main.cpp ${IMGUI_SOURCES})

target_include_directories(index PRIVATE ${IMGUI_DIR} ${IMGUI_DIR}/backends)

# Ensure output is .html
set_target_properties(index PROPERTIES SUFFIX ".html")