#pragma once

#include "Uttility.hpp"
#include "Collider.hpp"
#include "Animation.hpp"
#include "Input.hpp"
#include "World.hpp"

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Player : Collider {
    unsigned int m_spriteId;
    enum struct Moves { run, LAST };
    Animation<Moves> m_animation;
    static constexpr unsigned int MoveFrames[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    glm::mat4 m_mView;

    void Move(float _dT, const World& _world) {
        m_vel.y += 10.f * _dT;
        glm::vec2 target = m_pos + m_vel * _dT;
        m_pos.x = _world.MoveH(*this, target);
        m_pos.y = _world.MoveV(*this, target);
    }
public:
    Player() : Collider(glm::vec2(1)),  m_animation(), m_mView(1){
        m_animation.DefineClip(Moves::run, MoveFrames, sizeof(MoveFrames) / sizeof(*MoveFrames), 0.03f);
    }
    void Update(float _dT, const Input& _input, World& world) {
        m_animation.Update(_dT);
        m_spriteId = m_animation.GetSpireId();
        
        Move(_dT, world);
        m_mView = glm::translate(glm::mat4(1), glm::vec3(m_pos, 0));

        m_vel.x = 0;
        if(_input.KeyDown(Input::Inputs::Right)) m_vel.x += 3;
        if(_input.KeyDown(Input::Inputs::Left)) m_vel.x += -3;
    }
    const unsigned int* GetSpriteIdPtr() const { return &m_spriteId; }
    const float* GetMViewPtr() const { return glm::value_ptr(m_mView);}
};