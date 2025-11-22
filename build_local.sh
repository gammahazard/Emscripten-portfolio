#!/bin/bash
set -e

echo "========================================"
echo " 1. SETTING UP"
echo "========================================"
ROOT_DIR=$(pwd)
mkdir -p libs
mkdir -p dist/doom
mkdir -p build

# CLEANUP: Remove old broken builds to force a fresh start
rm -rf libs/Dwasm/build
rm -rf build
rm -rf dist/doom/*

echo "========================================"
echo " 2. PREPARING DWASM FILES"
echo "========================================"
if [ ! -d "libs/Dwasm" ]; then
    git clone https://github.com/GMH-Code/Dwasm.git libs/Dwasm
fi

# Ensure the special 'wasm/fs' folder exists
mkdir -p libs/Dwasm/wasm/fs

# Download WADs
if [ ! -f "libs/Dwasm/wasm/fs/doom1.wad" ]; then
    echo "--- Downloading doom1.wad ---"
    curl -L -o libs/Dwasm/wasm/fs/doom1.wad https://distro.ibiblio.org/slitaz/sources/packages/d/doom1.wad
fi

if [ ! -f "libs/Dwasm/wasm/fs/prboomx.wad" ]; then
    echo "--- Downloading prboomx.wad ---"
    curl -L -o libs/Dwasm/wasm/fs/prboomx.wad https://raw.githubusercontent.com/coelckers/prboom-plus/master/prboom2/data/prboom-plus.wad
fi

echo "========================================"
echo " 3. COMPILING DWASM (Software Mode)"
echo "========================================"
cd libs/Dwasm
mkdir -p build 
cd build

# --- KEY CHANGE ---
# We removed -DGL4ES_PATH. This forces Dwasm to build in Software Mode.
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release

emmake make

cd "$ROOT_DIR"

echo "--- Installing Doom ---"
cp libs/Dwasm/build/index.html dist/doom/index.html
cp libs/Dwasm/build/index.js dist/doom/
cp libs/Dwasm/build/index.wasm dist/doom/
cp libs/Dwasm/build/index.data dist/doom/

echo "========================================"
echo " 4. BUILDING PORTFOLIO"
echo "========================================"

# Check for ImGui
if [ ! -d "libs/imgui" ]; then
    echo "--- Cloning ImGui ---"
    git clone -b docking https://github.com/ocornut/imgui.git libs/imgui
fi

mkdir -p build
cd build
emcmake cmake ..
emmake make
cd "$ROOT_DIR"

echo "========================================"
echo " DONE. Run: cd dist && python3 -m http.server 8080"
echo "========================================"