#include <emscripten.h>
#include <chrono>

#include "shared.hpp"
#include "Player.hpp"
#include "Input.hpp"

bool first = true;
std::chrono::time_point<std::chrono::system_clock> last{};

Player player{};
Input::InputState lastInput;
Input::InputState activeInput;

int main() {
    shared::player_frameCount = player.GetSpriteIdPtr();
    shared::player_mView = player.GetMViewPtr();
    shared::input = &activeInput;
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
    const char* GetInputTitels() {
        return Input::Titels;
    }
    
    EMSCRIPTEN_KEEPALIVE
    void Pause() {
        first = true;
    }

    EMSCRIPTEN_KEEPALIVE
    void Update() {
        auto now = std::chrono::system_clock::now();
        if(!first) {
            const Input input(lastInput, activeInput);
            std::chrono::duration<float> d = now - last;
            float dT = d.count();
            player.Update(dT, input);
        } else { first = false; }
        last = now;
        lastInput = activeInput;
    }

    EMSCRIPTEN_KEEPALIVE
    unsigned int staticObjectsNumber() {
        return 2;
    }

    EMSCRIPTEN_KEEPALIVE
    const float* staticObjectsPos() {
        static constexpr float pos[] = {0,-1, 0,1};
        return pos;
    }

    EMSCRIPTEN_KEEPALIVE
    const unsigned int* staticObjectsSpriteIds() {
        static constexpr unsigned int ids[] = {0, 0};
        return ids;
    }

    EMSCRIPTEN_KEEPALIVE
    const float* staticObjectsAnimation() {
        // frame time, frames, offset (<1)
        static constexpr float ani[] = {0.1,4,0, 0.1,4,0.5};
        return ani;
    }
};