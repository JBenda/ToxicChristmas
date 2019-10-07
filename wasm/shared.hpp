#pragma once

#include "Input.hpp"

namespace shared {
    extern const unsigned int* player_frameCount;
    extern const float* player_mView;
    extern const Input::InputState* input;
    extern const float* mProjection;

    extern const unsigned int amount;
    extern void **shared[];
};
