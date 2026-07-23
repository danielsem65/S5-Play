# S5 Play

[![Build](https://github.com/danielsem65/S5-Play/actions/workflows/build.yml/badge.svg)](https://github.com/danielsem65/S5-Play/actions/workflows/build.yml)
[![Platform](https://img.shields.io/badge/platform-Windows%20x64-0078D4.svg)](#system-requirements)
[![Status](https://img.shields.io/badge/status-early%20development-orange.svg)](#current-status)
[![License](https://img.shields.io/badge/license-GPL--2.0-blue.svg)](LICENSE)

S5 Play is a free and open-source PlayStation 5 emulator for Windows, built on C++20 with a Vulkan backend.

Based on KytyPS5 (a heavily modified fork of the original Kyty project), with upstream fixes and improvements merged regularly.

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

## Screenshots

<table align="center">
  <tr>
    <td align="center">
      <strong>Disgaea 6</strong><br>
      <img src="docs/screenshots/ps5-01.png" width="300" alt="Disgaea 6 running in S5 Play">
    </td>
    <td align="center">
      <strong>Dreaming Sarah</strong><br>
      <img src="docs/screenshots/ps5-03.png" width="300" alt="Dreaming Sarah running in S5 Play">
    </td>
  </tr>
  <tr>
    <td align="center">
      <strong>Minecraft Legends</strong><br>
      <img src="docs/screenshots/ps5-04.png" width="300" alt="Minecraft Legends running in S5 Play">
    </td>
    <td align="center">
      <strong>SILENT HILL: The Short Message</strong><br>
      <img src="docs/screenshots/ps5-05.png" width="300" alt="SILENT HILL: The Short Message running in S5 Play">
    </td>
  </tr>
</table>

## Contributing

Testing games and submitting detailed bug reports are useful ways to contribute. Search existing
issues first, then use the **Game Emulation Bug Report** template and attach the complete log file.

Code contributions should be focused, build successfully on Windows, and include relevant tests
where practical.

### Formatting

Set up the clang-format hook after cloning:

```powershell
python -m pip install pre-commit
python -m pre_commit install --install-hooks
```

It formats staged `.cpp`, `.h`, and `.inc` files in `src`.

## Developer Information

The PS5 graphics architecture is based on AMD RDNA 2. Use AMD's
[RDNA 2 Instruction Set Architecture Reference Guide (document 70648)](https://docs.amd.com/v/u/en-US/rdna2-shader-instruction-set-architecture)
as the primary instruction-encoding reference when working on shader decoding and recompilation.

Important areas of the codebase:

- [`src/graphics/shader/recompiler`](src/graphics/shader/recompiler) — instruction decoding,
  intermediate representation, control flow, resource tracking, and SPIR-V emission
- [`src/graphics/guest_gpu`](src/graphics/guest_gpu) — PS5 (Prospero) GPU formats and command processing
- [`src/graphics/host_gpu`](src/graphics/host_gpu) — Vulkan host backend and resource management
- [`tests`](tests) — focused memory, shader, and resource-tracking regression tests

The renderer targets Vulkan 1.3.

## Building

### Build requirements

- Git
- CMake 3.12 or newer
- Ninja
- Visual Studio 2022 with **Desktop development with C++** workload and **C++ Clang tools for Windows**
- Qt 6.5+ (MSVC 2022), including Concurrent, Network, and Widgets
- Vulkan SDK 1.3+

The Microsoft C++ compiler (`cl.exe`) is not supported; use `clang-cl`.

Open a **Developer PowerShell for Visual Studio 2022** and run:

```powershell
# Install prerequisites:
#   - Visual Studio 2022 with "Desktop development with C++"
#   - Vulkan SDK 1.3+
#   - Qt 6.5+ (MSVC 2022)
#   - Ninja

git clone --recursive https://github.com/danielsem65/S5-Play.git
cd S5-Play
cmake -S src -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja -C build
```

### Pre-built binaries

Download from [GitHub Actions](https://github.com/danielsem65/S5-Play/actions) artifacts.

## License

GPL-2.0
