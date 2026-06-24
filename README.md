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

The preview app can import images, videos, Qt shader binaries (qsb) and local GLSL shader sources (frag/vert). GLSL sources are compiled to qsb in the user cache with `qsb-qt6` in order to preview. Shadertoy fragment snippets are converted automatically for Qt Shader Tools and baked into qsb. There is probably a better way to implement shadertoy shaders but I couldn't figure it out.

There are some example shaders under the /wallpapers directory which I used for testing.

![Demo gif](/assets/demo.gif)

## Install Plasma wallpaper locally

```sh
cmake --install build --prefix "$HOME/.local"
```

The wallpaper package installs to `$HOME/.local/share/plasma/wallpapers` which Plasma can discover through XDG data paths.

```sh
export QML2_IMPORT_PATH="$HOME/.local/lib64/qt6/qml${QML2_IMPORT_PATH:+:$QML2_IMPORT_PATH}"
plasmashell --replace
```

After installing or changing the import environment, restart Plasma Shell or log out and back in then select `BgEngine` from desktop wallpaper settings.

## Architecture

- `wallpapers/org.brtcrt.bgengine` is the Plasma wallpaper KPackage.
- `src/engine` exposes `BgEngine`, which is a QML module and C++ controller.
- `src/preview` is a test Qt Quick app for fast previews without KDE Desktop testing.
- `src/engine/shaders` contains GLSL compiled to `.qsb` during the build.
