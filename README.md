# AliceAndMe

A lightweight desktop companion widget that displays an anime character (Anya Forger) whose image changes based on the time of day. Built with **C++17**, **Dear ImGui**, and **DirectX 11**.

![Status](https://img.shields.io/badge/status-in_progress-yellow)

---

## What It Does

- Shows a **borderless, always-on-top** window (500×600) with an Anya image
- The image **automatically changes** based on your system clock (13 different images across 24 hours)
- Displays the **current time** in a pill-shaped overlay at the top ("3:42 PM")
- **Drag anywhere** to move the window
- **Press Escape** to close

---

## Tech Stack

| Component        | Technology                                             | Why                                                    |
| ---------------- | ------------------------------------------------------ | ------------------------------------------------------ |
| Language         | C++17                                                  | Fast, low memory, compiles to a single .exe            |
| UI Framework     | [Dear ImGui](https://github.com/ocornut/imgui) v1.91.8 | Immediate-mode GUI, tiny footprint, great for overlays |
| Graphics Backend | DirectX 11                                             | Native on every Windows 7+ machine, zero driver issues |
| Image Loading    | [stb_image.h](https://github.com/nothings/stb)         | Single-header PNG/JPEG decoder, no dependencies        |
| Window           | Win32 API                                              | Borderless popup with always-on-top and drag support   |
| Build System     | CMake + MinGW-w64 (GCC 15)                             | Generates a portable .exe with no DLL dependencies     |

---

## Project Structure

```
AliceAndMe/
├── CMakeLists.txt           # Build configuration
├── README.md                # This file
│
├── src/                     # Application source code
│   ├── main.cpp             # Entry point + render loop (~120 lines)
│   ├── window.h             # Win32 window creation + input handling
│   ├── renderer.h           # DirectX 11 device/swap chain setup
│   ├── texture_loader.h     # PNG/JPEG → DX11 GPU texture loader
│   └── time_map.h           # Hour → image mapping (13 time ranges)
│
├── external/                # Third-party dependencies (vendored)
│   ├── imgui/               # Dear ImGui v1.91.8 (core + DX11/Win32 backends)
│   └── stb/                 # stb_image.h (single-header image loader)
│
├── moods/                   # Character images
│   ├── time/                # 13 time-based PNGs + 3 emotion PNGs
│   └── extra/               # Additional Anya images (fallback, etc.)
│
├── old/                     # Previous Electron-based version (archived)
│   ├── main.js              # Old Electron main process
│   ├── index.html            # Old renderer entry
│   ├── components/          # Old JS components
│   └── config/              # Old JS config (timeMapConfig.js)
│
└── build/                   # CMake build output (gitignored)
    ├── AliceAndMe.exe       # The compiled app (~1.2 MB)
    └── moods/               # Copied assets
```

---

## Source Files Explained

### `src/main.cpp` — Entry point + render loop

The "conductor" of the app. Does 6 things in order:

1. Creates the Win32 window (via `window.h`)
2. Initializes DirectX 11 (via `renderer.h`)
3. Sets up Dear ImGui with DX11 + Win32 backends
4. Preloads all 13 time images into GPU memory (via `texture_loader.h`)
5. Runs the frame loop: check time → draw character → draw time pill → present
6. Cleans up everything on exit

### `src/window.h` — Win32 window + input

Creates the OS-level window with these properties:

- `WS_POPUP` → borderless (no title bar, no borders)
- `WS_EX_TOPMOST` → always stays above other windows
- `WM_NCHITTEST → HTCAPTION` → click-drag anywhere to move
- `VK_ESCAPE` → close the app
- Handles `WM_SIZE` to resize the DX11 render target

### `src/renderer.h` — DirectX 11 GPU plumbing

Sets up the connection between the app and the graphics card:

- **Device** → represents the GPU, creates textures
- **DeviceContext** → sends draw commands
- **SwapChain** → double-buffer (draw on back, flip to front)
- **RenderTarget** → the back-buffer texture we draw into each frame

### `src/texture_loader.h` — Image loading

Bridges the gap from disk files to ImGui rendering:

1. `stbi_load()` → decode PNG/JPEG pixels from disk
2. `CreateTexture2D()` → upload pixel data to GPU
3. `CreateShaderResourceView()` → make it drawable by ImGui
4. Returns a `Texture { srv, width, height }` struct

### `src/time_map.h` — Time-based image selection

Ported directly from the original `config/timeMapConfig.js`. Contains:

- 13 hour-range entries mapping to filenames (e.g., `{6, 8, "6am8am.png"}`)
- `GetImageForCurrentHour()` → returns the path for the current time
- `GetCurrentTimeString()` → returns "3:42 PM" for the overlay

#### Time-to-Image Map

| Hours               | Image          |
| ------------------- | -------------- |
| 12:00 AM – 1:00 AM  | `12am1am.png`  |
| 1:00 AM – 6:00 AM   | `1am6am.png`   |
| 6:00 AM – 8:00 AM   | `6am8am.png`   |
| 8:00 AM – 10:00 AM  | `8am10am.png`  |
| 10:00 AM – 12:00 PM | `10am12pm.png` |
| 12:00 PM – 1:00 PM  | `12am1pm.png`  |
| 1:00 PM – 2:00 PM   | `1pm2pm.png`   |
| 2:00 PM – 4:00 PM   | `2pm4pm.png`   |
| 4:00 PM – 6:00 PM   | `4pm6pm.png`   |
| 6:00 PM – 7:00 PM   | `6pm7pm.png`   |
| 7:00 PM – 8:00 PM   | `7pm8pm.png`   |
| 8:00 PM – 11:00 PM  | `8pm11pm.png`  |
| 11:00 PM – 12:00 AM | `11pm12am.png` |

---

## Building

### Prerequisites

- **MinGW-w64** (GCC 13+) — install via `choco install mingw`
- **CMake 3.16+** — install via `choco install cmake`

### Build Commands

```powershell
# Configure (first time only)
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release
```

### Run

```powershell
.\build\AliceAndMe.exe
```

The exe is ~1.2 MB and has zero DLL dependencies (CRT is statically linked).

---

## Controls

| Input                 | Action          |
| --------------------- | --------------- |
| Click + drag anywhere | Move the window |
| Escape                | Close the app   |

---

## What's Done

- [x] Project scaffolded with CMake + MinGW
- [x] Dear ImGui v1.91.8 + DX11/Win32 backends integrated
- [x] Borderless, always-on-top, draggable Win32 window
- [x] All 13 time-based images preloaded into GPU at startup
- [x] Automatic image switching based on system clock (checked every minute)
- [x] Aspect-ratio-preserving image scaling, centered in window
- [x] Time pill overlay showing current time ("3:42 PM")
- [x] Escape key to close
- [x] Codebase split into 5 focused files with documentation
- [x] Builds to a single portable .exe (~1.2 MB)

## What's Next

- [ ] Speech bubbles showing system stats (CPU %, RAM usage)
- [ ] Embed images inside the .exe (no loose moods/ folder needed)
- [ ] Transparent window background (only the character visible, no dark bg)
- [ ] Hover interactions (random image swap on mouse hover)
- [ ] System tray icon with right-click menu
- [ ] Auto-start on Windows login
