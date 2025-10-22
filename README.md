# 🃏 Freecell

A live instance is running on [gnikdroy.github.io/freecell](https://gnikdroy.github.io/freecell)

Freecell implementation in C. Play Freecell in a native build or in the browser (WebAssembly). This repo contains the game core, simple rendering/platform glue, and build rules using CMake + Ninja.

This implementation uses the same RNG as the original Microsoft FreeCell, so game deal numbers match the original implementation.

## ⚡ Quick summary
- Language: C
- Build system: CMake + Ninja
- Supported toolchains: Clang (native) and Emscripten (WASM) — only these two are tested.

## ✅ Requirements
- CMake (>= 3.15)
- Ninja
- Clang (native builds)
- Emscripten SDK (for web builds)
- Python (used by build generation steps, e.g., GLAD)

## 🛠️ Build instructions

1) Native (Clang + Ninja)
```sh
# create and enter build dir
mkdir build
cd build

# configure with Clang explicitly
cmake -G Ninja -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..

# build the app and tests
cmake --build . --config Release --target freecell
cmake --build . --config Release --target tests
```
Result: executable(s) in `build/` (e.g. `build/Freecell.exe` on Windows).

2) 🌐 Web (Emscripten / WASM)
```sh
mkdir embuild
cd embuild

# use emcmake to set the emscripten toolchain for CMake
emcmake cmake -G Ninja ..
cmake --build . --config Release --target freecell
```
Result: JS/WASM artifacts and Ninja manifest live in `embuild/`.

## ▶️ How to play

Basic Freecell rules (as implemented here):
- Layout: 8 tableau columns, 4 free cells, 4 foundation piles (one per suit).
- Objective: move all cards to the foundation piles, Ace → King in suit order.
- Moves:
  - Move the top card of any tableau column to a free cell, another tableau column, or a foundation (if it fits).
  - You may move a sequence of cards between tableau columns if there are enough empty free cells and/or empty tableau columns to perform the move (standard Freecell sequence-move rule).
  - Free cells hold one card each.
- Deal numbers: deals are reproducible and match the original Microsoft FreeCell numbering thanks to the identical RNG.

Controls: use the UI produced by the build (native or web). The UI offers dealing/new game and card interaction; exact controls depend on the build target (native window vs browser).

## ▶️ Running
- Native: run the produced binary, e.g.:
```sh
./build/Freecell.exe
```
- Web: open the generated HTML/JS from `embuild/` in a browser (or serve the directory).

## 🧪 Tests
- Native tests: run `build/tests.exe` (or equivalent binary produced by the build).
- Web tests: see the Emscripten outputs in `embuild/` (e.g. `tests.js` if generated).

## 🗂️ Project structure
- CMakeLists.txt — top-level CMake configuration and targets.
- src/ — source code
  - src/game/ — game logic
  - src/core/ — utilities and single-file third-party sources
  - src/rendering/, src/platform/ — rendering and platform glue
- include/ — public headers

## 🤝 Contributing
- Follow existing C style.
- Test both Clang native and Emscripten builds after changes.
- Open issues or PRs for bugs or feature requests.
