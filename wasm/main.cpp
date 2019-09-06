#include <emscripten.h>
#include <iostream>
#include <chrono>
#include "shared.hpp"


unsigned int frameCount = 20;
constexpr unsigned int amtFrame = 23;
constexpr double frameTime = 0.03;
double frameTimer = 0;

bool first = false;
std::chrono::time_point<std::chrono::system_clock> last{};

int main() {
    shared::player_frameCount = &frameCount;
    emscripten_run_script("BindASM();");
    return 0;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    unsigned int SharedPointerLength() {
        return shared::amount;
    }

    EMSCRIPTEN_KEEPALIVE
    void*** GetSharedPointer() {
        return shared::shared;
    }

    EMSCRIPTEN_KEEPALIVE
    void Update() {
        auto now = std::chrono::system_clock::now();
        if(!first) {
            std::chrono::duration<double> d = now - last;
            double dT = d.count();
            frameTimer += dT;
            if (frameTimer > frameTime) {
                frameTimer = 0;
                if(++frameCount == amtFrame)
                    frameCount = 0;
            }
        } else { first = false; }
        last = now;
    }
};