# C++ WebAssembly Engine (Wasm)

![Build Status](https://github.com/gammahazard/Emscripten-portfolio/actions/workflows/main.yml/badge.svg)
![Platform](https://img.shields.io/badge/platform-WebAssembly-blue)
![License](https://img.shields.io/badge/license-MIT-green)

A high-performance, serverless web application built with **C++17** and **WebAssembly**, rendering a desktop-grade UI directly to the browser using **Dear ImGui** and **SDL2**. 

This project demonstrates a full CI/CD pipeline where C++ source code is compiled into Wasm artifacts via **GitHub Actions** and automatically deployed to the edge using **Vercel**.

### 🔗 [Live Demo](https://your-project-name.vercel.app)

---

## 🏗 Architecture

Unlike traditional web apps that rely on the DOM and JavaScript heavy-lifting, this application runs a compiled binary in the browser.

* **Core Logic:** C++17
* **Compilation:** Emscripten (Clang-based toolchain)
* **Rendering:** OpenGL ES 3.0 via SDL2 & Dear ImGui
* **Build System:** CMake
* **CI/CD:** GitHub Actions (Automated Cloud Build)
* **Hosting:** Vercel (Static Serving of Wasm binaries)

```mermaid
graph TD;
    A[C++ Source Code] -->|CMake| B(GitHub Actions Runner);
    B -->|Emscripten Compiler| C[main.wasm / main.js];
    C -->|Vercel CLI| D[Vercel Edge Network];
    D -->|Downloads| E[User Browser];
    E -->|Executes| F[Native Performance UI];

    ## 🚀 Features

* **Near-Native Performance:** Leveraging **WebAssembly (Wasm)** to execute complex C++ logic at speeds approaching native machine code, bypassing JavaScript bottlenecks.
* **Immediate Mode GUI:** Utilizes **Dear ImGui** to render a responsive, frame-based UI architecture. This is the industry standard for game engines, internal tools, and high-frequency visualizations.
* **Virtual File System (VFS):** Implements an in-memory file system (MEMFS) allowing the C++ code to read/write files and assets transparently within the browser environment.
* **Automated Pipeline:** Zero-touch deployment. Pushing code to GitHub triggers a cloud compilation and automatically deploys the binaries to Vercel's edge network.
* **Custom WASM Headers:** configured for Cross-Origin Opener Policy (COOP) and Cross-Origin Embedder Policy (COEP) to ensure security compliance and enable high-performance features.

## 🛠 Local Development

To build this project locally, you need the **Emscripten SDK** installed.

### Prerequisites
* Git
* CMake (3.10+)
* Emscripten (`emsdk`)

### Build Steps

1.  **Clone the Repository**
    *Note: The `--recursive` flag is critical to download the Dear ImGui submodule.*
    ```bash
    git clone --recursive [https://github.com/yourusername/repo-name.git](https://github.com/yourusername/repo-name.git)
    cd repo-name
    ```

2.  **Compile with Emscripten**
    We use `emcmake` to configure the build environment for the Web.
    ```bash
    mkdir build
    cd build
    emcmake cmake ..
    emmake make
    ```

3.  **Run Locally**
    Wasm files require specific MIME types that standard local servers often miss. Use Emscripten's built-in runner:
    ```bash
    # Runs a local server at localhost:8080
    emrun ../dist/index.html
    ```

## ⚙️ DevOps & Deployment

This project uses **GitHub Actions** to eliminate "works on my machine" issues. The CI/CD workflow is defined in `.github/workflows/main.yml`.

### The Pipeline
1.  **Trigger:** On push to `main`.
2.  **Environment:** Ubuntu Latest (runs inside a container).
3.  **Process:**
    * Checkout code & submodules.
    * Install Emscripten SDK (pinned version for stability).
    * **Compile:** Builds `src/main.cpp` into `dist/index.html` and `dist/index.wasm`.
    * **Configure:** Injects `vercel.json` to handle Wasm headers.
    * **Deploy:** Uploads the `dist/` folder via Vercel CLI with production tokens.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.