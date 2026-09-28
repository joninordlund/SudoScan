#!/bin/bash

set -e

# Always work from the SudoScan project directory
cd "$(dirname "$0")"

echo "==> Building Release..."
cmake --build build-release -j$(nproc)

echo "==> Installing into AppDir..."
cmake --install build-release --prefix "$PWD/AppDir/usr"

echo "==> Building AppImage..."
LD_LIBRARY_PATH="$HOME/tesseract/build-shared:$HOME/Qt/6.8.0/gcc_64/lib" \
QMAKE="$HOME/Qt/6.8.0/gcc_64/bin/qmake" \
./linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --plugin qt \
    --output appimage

echo
echo "==> Done!"
ls -lh SudoScan-x86_64.AppImage


