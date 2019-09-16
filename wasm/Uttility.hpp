#pragma once

#define DEBUG

#include <glm/vec2.hpp>

namespace Utillity {
    constexpr float StaticObjectSize = 1;
    
    struct Rect {
        Rect(glm::vec2 downLeft, glm::vec2 size) : 
            downLeft{downLeft}, size{size}, topRight{downLeft + size} {}
        glm::vec2 downLeft;
        glm::vec2 size;
        glm::vec2 topRight;
    };
}