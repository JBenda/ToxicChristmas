~/emsdk/fastcomp/emscripten/em++ wasm/*.cpp -o wasmLoader.js -O1 -s WASM=1 -s NO_EXIT_RUNTIME=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']" -std=c++17
