#include <emscripten.h>
#include <chrono>

#include "shared.hpp"
#include "Player.hpp"
#include "Input.hpp"

bool first = true;
std::chrono::time_point<std::chrono::system_clock> last{};

Player player{};

int main() {
    shared::player_frameCount = player.GetSpriteIdPtr();
    shared::player_mView = player.GetMViewPtr();
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
            std::chrono::duration<float> d = now - last;
            float dT = d.count();
            player.Update(dT, Input());
        } else { first = false; }
        last = now;
    }
};