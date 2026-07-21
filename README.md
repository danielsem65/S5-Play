# S5 Play

[![Build](https://github.com/danielsem65/S5-Play/actions/workflows/build.yml/badge.svg)](https://github.com/danielsem65/S5-Play/actions/workflows/build.yml)
[![Platform](https://img.shields.io/badge/platform-Windows%20x64-0078D4.svg)](#system-requirements)
[![Status](https://img.shields.io/badge/status-early%20development-orange.svg)](#current-status)
[![License](https://img.shields.io/badge/license-GPL--2.0-blue.svg)](LICENSE)

S5 Play is a free and open-source PlayStation 5 emulator for Windows, built on C++20 with a Vulkan backend.

Based on KytyPS5, which itself is a heavily modified fork of the original Kyty project.

> [!IMPORTANT]
> S5 Play is not affiliated with Sony Interactive Entertainment or PlayStation. Use only game files
> that you have obtained legally.

## Current Status

Early development. Can boot 2D games and some 3D titles. Expect crashes, graphical glitches, and
low performance.

## System Requirements

- **OS:** Windows 10/11 x64
- **CPU:** x86-64 with AVX2 support (modern Intel/AMD)
- **GPU:** Vulkan 1.3 capable (NVIDIA GTX 900+, AMD RX 400+, Intel Arc)
- **RAM:** 16 GB minimum

## Building

### From source

```powershell
# Install prerequisites:
#   - Visual Studio 2022 with "Desktop development with C++"
#   - Vulkan SDK 1.3+
#   - Qt 6.5+ (MSVC 2022)
#   - Ninja

git clone --recursive https://github.com/danielsem65/S5-Play.git
cd S5-Play
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

### Pre-built binaries

Download from [GitHub Actions](https://github.com/danielsem65/S5-Play/actions) artifacts.

## License

GPL-2.0
