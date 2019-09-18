#include <emscripten.h>
#include <chrono>

#include "shared.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "World.hpp"
#include "Object.hpp"
#include <glm/vec2.hpp>
#include <memory>

bool first = true;
std::chrono::time_point<std::chrono::system_clock> last{};

Player player{};
Input::InputState lastInput;
Input::InputState activeInput;

World world{};

int main() {
    std::shared_ptr<Level>lvl = std::make_shared<Level>(
        glm::vec2(0, 0),
        Utillity::Rect(glm::vec2(3, 0), glm::vec2(1, 1))
    );
    lvl->statics.push_back(new Box({0, 2}));
    lvl->statics.push_back(new Box(glm::vec2(4, 0)));
    
    world.LoadLevel(lvl);

    shared::player_frameCount = player.GetSpriteIdPtr();
    shared::player_mView = player.GetMViewPtr();
    shared::input = &activeInput;
    Box box(glm::vec2(0, 1));

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
            player.Update(dT, input, world);
        } else { first = false; }
        last = now;
        lastInput = activeInput;
    }

    EMSCRIPTEN_KEEPALIVE
    unsigned int staticObjectsNumber() {
        std::cout << "obj count: " << world.GetStaticObjects().size() << '\n';
        return world.GetStaticObjects().size();
    }

    EMSCRIPTEN_KEEPALIVE
    const float* staticObjectsPos() {
        static float *pos = nullptr;
        if (pos) delete[] pos;

        if(!pos) {
            auto objs = world.GetStaticObjects();
            pos = new float[objs.size() * 2];
            float* fItr = pos;
            for(const StaticObject* o: objs) {
                const glm::vec2& p = o->GetPos();
                *(fItr++) = p.x;
                *(fItr++) = p.y;
            }
        }
        return pos;
    }

    EMSCRIPTEN_KEEPALIVE
    const unsigned int* staticObjectsSpriteIds() {
        static unsigned int *sids = nullptr;
        if(sids) delete[] sids;

        if(!sids) {
            auto objs = world.GetStaticObjects();
            sids = new unsigned int[objs.size()];
            unsigned int* uiItr = sids;
            for(const StaticObject* o : objs) {
                *(uiItr++) = o->spriteId + 1;
            }
        }
        return sids;
    }

    EMSCRIPTEN_KEEPALIVE
    const float* staticObjectsAnimation() {
        // frame time, frames, offset (<1)
        static float *ani = nullptr;
        if(ani) delete[] ani;

        if(!ani) {
            auto objs = world.GetStaticObjects();
            ani = new float[objs.size() * 3];
            float* fItr = ani;
            for(const StaticObject* o : objs) {
                *(fItr++) = o->frameTime;
                *(fItr++) = o->frames;
                *(fItr++) = o->offset;
            }
        }
        return ani;
    }
};