$files = Get-ChildItem -Path wasm/* -Include *.cpp -Name | foreach {"wasm/$($_)"}
..\..\..\..\webgl\emsdk\fastcomp\emscripten\em++ $files -o wasmLoader.js -O3 -s WASM=1 -s NO_EXIT_RUNTIME=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['cwrap']" -std=c++17