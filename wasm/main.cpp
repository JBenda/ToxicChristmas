#include <emscripten.h>
#include "shared.cpp"

unsigned int frameCount = 0;

int main() {
    shared::player_frameCount = &frameCount;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
unsigned int SharedPointerLength() {
    return shared::amount;
}

/**
 * @return array with construcdet: [length, pptr0, pptr1, pptr2, ...]
 */
EMSCRIPTEN_KEEPALIVE
void** GetSharedPointer(unsigned int id) {
    return shared::shared[id];
}