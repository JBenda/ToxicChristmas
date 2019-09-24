#pragma once

#define DEBUG

#include <glm/vec2.hpp>

namespace Utillity {
    constexpr float StaticObjectSize = 1;
    constexpr float epsilon = 0.00001f;
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