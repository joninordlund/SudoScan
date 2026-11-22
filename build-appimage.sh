#!/bin/bash

# Always work from the SudoScan project directory
cd "$(dirname "$0")"

echo "==> Building Release..."
cmake --build build-release -j"$(nproc)" || exit 1

echo "==> Installing into AppDir..."
cmake --install build-release --prefix "$PWD/AppDir/usr" || exit 1

echo "==> Building AppImage..."
LD_LIBRARY_PATH="$HOME/tesseract/build-shared:$HOME/Qt/6.8.0/gcc_64/lib" 
QMAKE="$HOME/Qt/6.8.0/gcc_64/bin/qmake" 
./linuxdeploy-x86_64.AppImage 
    --appdir AppDir 
    --plugin qt 
    --desktop-file packaging/sudoscan.desktop 
    --icon-file packaging/sudoscan.png 
    --output appimage || exit 1

echo
echo "==> Done!"
ls -lh SudoScan-x86_64.AppImage
