#!/usr/bin/env bash
# Setup Emscripten SDK for building Autumn WASM artifacts

set -e

EMSDK_DIR="emsdk"

# Check if emsdk is already installed
if [ -d "$EMSDK_DIR" ]; then
    echo "✓ Emscripten SDK directory already exists at $EMSDK_DIR"
    echo "  Updating to latest version..."
    cd "$EMSDK_DIR"
    git pull
    ./emsdk install latest
    ./emsdk activate latest
    echo "✓ Emscripten SDK updated and activated"
else
    echo "Installing Emscripten SDK..."
    # Clone the Emscripten SDK repository
    git clone https://github.com/emscripten-core/emsdk.git
    cd "$EMSDK_DIR"

    # Install and activate the latest SDK tools
    ./emsdk install latest
    ./emsdk activate latest
    echo "✓ Emscripten SDK installed and activated"
fi

echo ""
echo "To use Emscripten in your current shell, run:"
echo "  source $EMSDK_DIR/emsdk_env.sh"
