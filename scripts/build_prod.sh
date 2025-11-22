#!/bin/bash
set -e # Exit immediately if anything fails

echo "========================================"
echo " 1. CI SETUP"
echo "========================================"
ROOT_DIR=$(pwd)

# Clean start
rm -rf libs/Dwasm/build
rm -rf build
rm -rf dist

mkdir -p libs
mkdir -p dist/doom
mkdir -p build

echo "========================================"
echo " 2. PREPARING DWASM & WADS"
echo "========================================"
if [ ! -d "libs/Dwasm" ]; then
    echo "--- Cloning Dwasm ---"
    git clone https://github.com/GMH-Code/Dwasm.git libs/Dwasm
fi

mkdir -p libs/Dwasm/wasm/fs

# A. DOWNLOAD GAME WAD (Shareware Levels)
# We keep downloading this because it's stable, but you could commit this too if you wanted.
if [ ! -f "libs/Dwasm/wasm/fs/doom1.wad" ]; then
    echo "--- Downloading doom1.wad ---"
    curl -L -o libs/Dwasm/wasm/fs/doom1.wad https://distro.ibiblio.org/slitaz/sources/packages/d/doom1.wad
fi

# B. COPY ENGINE WAD (From Root)
# Instead of downloading, we copy the file you manually placed in the root.
if [ -f "$ROOT_DIR/prboomx.wad" ]; then
    echo "--- Copying local prboomx.wad to Dwasm fs ---"
    cp "$ROOT_DIR/prboomx.wad" libs/Dwasm/wasm/fs/prboomx.wad
else
    echo "ERROR: prboomx.wad was not found in the project root!"
    echo "Please ensure you have committed 'prboomx.wad' to your repository."
    exit 1
fi

echo "========================================"
echo " 3. COMPILING DOOM (Software Mode)"
echo "========================================"
cd libs/Dwasm
mkdir -p build 
cd build

# Software Mode Build
emcmake cmake .. -DCMAKE_BUILD_TYPE=Release
emmake make

# Install to dist
cd "$ROOT_DIR"
echo "--- Installing Doom Artifacts ---"
cp libs/Dwasm/build/index.html dist/doom/index.html
cp libs/Dwasm/build/index.js dist/doom/
cp libs/Dwasm/build/index.wasm dist/doom/
cp libs/Dwasm/build/index.data dist/doom/

echo "========================================"
echo " 4. BUILDING PORTFOLIO (ImGui)"
echo "========================================"

# Clone ImGui if missing
if [ ! -d "libs/imgui" ]; then
    echo "--- Cloning ImGui (Docking Branch) ---"
    git clone -b docking https://github.com/ocornut/imgui.git libs/imgui
fi

cd build
emcmake cmake ..
emmake make
cd "$ROOT_DIR"

# Copy vercel configuration
cp vercel.json dist/

echo "========================================"
echo " BUILD COMPLETE"
echo "========================================"