# bgengine

`bgengine` is a KDE Plasma 6 rendering engine for desktop wallpapers with support for shader and media based files.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
```

## Preview

```sh
./build/bin/bgengine-preview
```

## Install Plasma wallpaper locally

```sh
cmake --install build --prefix "$HOME/.local"
```

The wallpaper package installs to `$HOME/.local/share/plasma/wallpapers`, which Plasma can discover through XDG data paths. The native `BgEngine` QML module installs to `$HOME/.local/lib64/qt6/qml`

```sh
export QML2_IMPORT_PATH="$HOME/.local/lib64/qt6/qml${QML2_IMPORT_PATH:+:$QML2_IMPORT_PATH}"
plasmashell --replace
```

After installing or changing the import environment, restart Plasma Shell or log out and back in, then select `BgEngine` from desktop wallpaper settings.

## Architecture

- `wallpapers/org.brtcrt.bgengine` is the Plasma wallpaper KPackage.
- `src/engine` exposes `BgEngine`, which is a QML module and C++ controller.
- `src/preview` is a test Qt Quick app for fast previews without KDE Desktop testing.
- `src/engine/shaders` contains GLSL compiled to `.qsb` during the build.
