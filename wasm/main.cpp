#include <emscripten.h>
#include <chrono>

#include "shared.hpp"
#include "Player.hpp"
#include "Input.hpp"
#include "World.hpp"
#include "Object.hpp"
#include "LevelLoader.hpp"
#include "Camera.hpp"
#include <glm/vec2.hpp>
#include <memory>


bool first = true;
std::chrono::time_point<std::chrono::system_clock> last{};

Player player{};
Input::InputState lastInput;
Input::InputState activeInput;

World world{};
Camera camera({-5.f * Utillity::ratio, 5.f}, {5.f * Utillity::ratio, -5.f});

int main() {
    shared::mProjection = camera.GetProjectionMatrix(); 
    std::shared_ptr<Level>lvl = std::make_shared<Level>(
        80, // w
        10, // h
        glm::vec2(-5, -5), // origin
        glm::vec2(0, 0), // start pos
        Utillity::Rect(glm::vec2(3, 0), glm::vec2(1, 1)) // target area
    );
    for(int i = 0; i < 20; ++i) {
        lvl->statics.push_back(LevelLoader::Instanciate(LevelLoader::Tiles::Box, {i * 2, 2}));
    }
    
    world.LoadLevel(lvl);

    shared::player_frameCount = player.GetSpriteIdPtr();
    shared::player_mView = player.GetMViewPtr();
    shared::player_flipSprite = player.GetIsFlippedPtr();
    shared::input = &activeInput;
    Box box(glm::vec2(0, 1));

    emscripten_run_script("BindASM();");
    return 0;
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void LoadLevel(unsigned char const * levelData, unsigned int amtPx, unsigned int width, unsigned int height) {
        std::shared_ptr<Level>lvl = std::make_shared<Level>(
            width * 2,
            height * 2,
            glm::vec2(0,0),
            glm::vec2(0,0),
            Utillity::Rect({3,0},{1,1})
        );
        unsigned char const * itr = levelData;
        for(unsigned int y = 0; y < height; ++y) {
            for(unsigned int x = 0; x < width; ++x) {
                unsigned char
                    r = *(itr++),
                    g = *(itr++),
                    b = *(itr++),
                    a = *(itr++);
                for (unsigned int i = 0; i < static_cast<unsigned int>(LevelLoader::Tiles::LAST); ++i) {
                    if(a == 255 && r == LevelLoader::ColorEncoding[i]) {
                        lvl->statics.push_back(LevelLoader::Instanciate(static_cast<LevelLoader::Tiles>(i), {x * 2 + 1, y * 2 + 1}));
                    }
                }
            }
        }
        world.LoadLevel(lvl);
    }

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
    float Update() {
        float dT = 0;
        auto now = std::chrono::system_clock::now();
        if(!first) {
            const Input input(lastInput, activeInput);
            std::chrono::duration<float> d = now - last;
            dT = d.count();
            player.Update(dT, input, world);
            camera.Update(dT, input, player, world);
        } else { first = false; }
        last = now;
        lastInput = activeInput;
        return dT;
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
                *(uiItr++) = o->spriteId;
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
