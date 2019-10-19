#import "shared.hpp"

#import "Input.hpp"

namespace shared {

const char str_player_frameCount[] = "player_frameCount";
const unsigned int *player_frameCount = nullptr;

const char str_player_flipSprite[] = "player_flipSprite";
const bool* player_flipSprite = nullptr;

const char str_player_mView[] = "player_mView";
const float* player_mView = nullptr;

const char str_input[] = "input";
const Input::InputState* input = nullptr;

const char str_mProjection[] = "projection";
const float* mProjection = nullptr;

void **shared[] = {
    (void**)str_player_frameCount,
    (void**)&player_frameCount,
    (void**)str_player_flipSprite,
    (void**)&player_flipSprite,
    (void**)str_player_mView,
    (void**)&player_mView,
    (void**)str_input,
    (void**)&input,
    (void**)str_mProjection,
    (void**)&mProjection
};
const unsigned int amount = sizeof(shared)  / sizeof(*shared)  / 2;
};
