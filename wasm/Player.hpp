#pragma once

#include "Uttility.hpp"
#include "Collider.hpp"
#include "Animation.hpp"
#include "Input.hpp"

#include <glm/vec2.hpp>

class Player : Collider {
    unsigned int m_spriteId;
    enum struct Moves { run, LAST };
    Animation<Moves> m_animation;
    static constexpr unsigned int MoveFrames[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
public:
    Player() : Collider(glm::vec2(1)),  m_animation(){
        m_animation.DefineClip(Moves::run, MoveFrames, sizeof(MoveFrames) / sizeof(*MoveFrames), 0.03f);
    }
    void Update(float _dT, const Input& _input) {
        m_animation.Update(_dT);
        m_spriteId = m_animation.GetSpireId();
    }
    unsigned int* GetSpriteIdPtr() { return &m_spriteId; }
};