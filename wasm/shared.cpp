#import "shared.hpp"

#import "Input.hpp"

namespace shared {

const char str_player_frameCount[] = "player_frameCount";
const unsigned int *player_frameCount = nullptr;

const char str_player_mView[] = "player_mView";
const float* player_mView = nullptr;

const char str_input[] = "input";
const Input::InputState* input = nullptr;

const unsigned int amount = 3;
void **shared[] = {
    (void**)str_player_frameCount,
    (void**)&player_frameCount,
    (void**)str_player_mView,
    (void**)&player_mView,
    (void**)str_input,
    (void**)&input};

};