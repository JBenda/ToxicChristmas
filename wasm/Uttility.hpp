#pragma once

#define DEBUG

#define REF

#include <glm/vec2.hpp>

namespace Utillity {
    constexpr float StaticObjectSize = 1;
    constexpr float epsilon = 0.00001f;
    constexpr float gravity = 20.f;
    constexpr float jumpPower = 8.0f;

    namespace Player {
        constexpr float a_ground = 5;
        constexpr float a_air = 3;
        constexpr float maxSpeed = 20;
        constexpr float xSpeedTransform = 0.3f; ///< part of horizontel speed transformed in heigh 
        constexpr float slowdown = 0.7f; ///< g * slowdon = g
        constexpr unsigned int lastContactForJump = 10;
    }

    constexpr float ratio = 4.f / 3.f;

    template<typename T>
    struct OrderReverser {
        OrderReverser(const T& _comp) : comp{_comp}{}
        bool operator() (const typename T::Type& _t1, const typename T::Type& _t2) const {
            return !comp(_t1, _t2);
        }
    private:
        const T& comp;
    };
    struct UpdateRateToSlow {};
    struct Rect {
        Rect(glm::vec2 downLeft, glm::vec2 size) : 
            downLeft{downLeft}, size{size}, topRight{downLeft + size} {}
        glm::vec2 downLeft;
        glm::vec2 size;
        glm::vec2 topRight;
    };
}
