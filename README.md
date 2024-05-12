![](artwork/ECLIPSE-banner.jpg)

# Eclipse Engine

[![CodeFactor](https://www.codefactor.io/repository/github/sherry65-code/eclipseengine/badge)](https://www.codefactor.io/repository/github/sherry65-code/eclipseengine)

> **NOTE**
>
> This Project is Currently Under Development!

Licensed under MIT License.

## How to build

> Only works on Windows and Linux! Mac systems not supported!

### Windows

```ps
git clone https://github.com/Sherry65-code/EclipseEngine --depth 1 --recursive
cd EclipseEngine
mkdir build
cd build
cmake ..
.\EclipseEngine.sln
```

### Linux

```bash
git clone https://github.com/Sherry65-code/EclipseEngine --depth 1 --recursive
cd EclipseEngine
mkdir build
cd build
cmake ..
make
make Shaders
```

## Linux Packages required to build this project

### Debian

```bash
sudo apt install libwayland-dev libxkbcommon-dev xorg-dev
```

### Fedora/RHEL

```bash
sudo dnf install wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel
```

### FreeBSD

```bash
pkg install wayland libxkbcommon evdev-proto xorgproto
```

## Requirements

C++ compiler (20 or later), CMake, (any build system), Vulkan SDK

Give a ⭐ if you want to support this project. We do not take donations or funds!