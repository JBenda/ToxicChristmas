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

#include <optional>
#include <utility>

class Player : public Collider {
    unsigned int m_spriteId {0};
    bool m_flippSprite {true};
    enum struct Moves { runR, jump, up, down, idle, LAST };
    Animation<Moves> m_animation;
    static constexpr unsigned int MoveFrames[] = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
    static constexpr unsigned int JumpFrames[] = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
    static constexpr unsigned int UpFrames[] = {19};
    static constexpr unsigned int DownFrames[] = {19};
    static constexpr unsigned int IdleFrames[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
#   define ARR_E(Arr) Arr, sizeof(Arr) / sizeof(*Arr)
    static constexpr std::tuple<const unsigned int*, std::size_t, const float>
        Frames[] = {
            {ARR_E(MoveFrames), 0.05f},
            {ARR_E(JumpFrames), 0.05f},
            {ARR_E(UpFrames), 0.2f},
            {ARR_E(DownFrames), 0.2f},
            {ARR_E(IdleFrames), 0.05f}
        };
    glm::mat4 m_mView;
    unsigned int m_grounded = 0;
    void Move(float _dT, const World& _world) {
        if(m_grounded) --m_grounded;

        glm::vec2 diff = m_vel * _dT;
        std::optional<float> res = _world.MoveH(*this, m_pos + glm::vec2(diff.x, 0), REF m_pos.y);
        if(res) {
            m_pos.x = res.value();
            m_vel.x = 0;
        } else {
            m_pos.x = m_pos.x + diff.x;
        }

        res = _world.MoveV(*this, m_pos + glm::vec2(0, diff.y));
        if(res) {
            m_pos.y = res.value();
            m_vel.y = 0;
        } else {
            m_pos.y = m_pos.y + diff.y;
        }

        if(_world.IsGrounded()) {
            m_grounded = Utillity::Player::lastContactForJump;
        }
    }
    enum Movment { jump, fall, run, LAST };
    using MovmentFlags = std::bitset<Movment::LAST>;
    MovmentFlags lastMoves{0};
public:
    Player() : Collider(glm::vec2(0.7f, 0.9f)),  m_animation(), m_mView(1){
        for(unsigned int i = 0; i < static_cast<unsigned int>(Moves::LAST); ++i) {
            m_animation.DefineClip(
                static_cast<Moves>(i),
                std::get<0>(Frames[i]),
                std::get<1>(Frames[i]),
                std::get<2>(Frames[i]));
        }
    }
    void Update(float _dT, const Input& _input, World& world) {
        MovmentFlags moves(0);

        m_animation.Update(_dT);
        m_spriteId = m_animation.GetSpireId();

        // input to movent
        glm::vec2 a{0};
        if(_input.KeyDown(Input::Inputs::Right)) {
            moves.set(Movment::run);
            m_flippSprite = false;
            a.x += Utillity::Player::a_ground;
        }

        if(_input.KeyDown(Input::Inputs::Left)) {
            m_flippSprite = true;
            moves.set(Movment::run);
            a.x -= Utillity::Player::a_ground;
        }
        
        if(_input.KeyPressed(Input::Inputs::Jump) && m_grounded) {
            moves.set(Movment::jump);
            m_grounded = 0;
            m_vel.y -= Utillity::jumpPower + Utillity::Player::xSpeedTransform * std::fabs(m_vel.x);
            m_vel.x *= (1.0f - Utillity::Player::xSpeedTransform);
        }
        m_vel.y += Utillity::gravity
            * _dT
            * (_input.KeyDown(Input::Inputs::Jump)
               ? Utillity::Player::slowdown
               : 1.0f);
        ;
        m_vel += a * _dT;
        
        if(     (!_input.KeyDown(Input::Inputs::Right) && !std::signbit(m_vel.x))
            ||  (!_input.KeyDown(Input::Inputs::Left) && std::signbit(m_vel.x))
            )  { m_vel.x *= std::pow(0.05f, _dT); }

        if(std::fabs(m_vel.x) > Utillity::Player::maxSpeed) {
            m_vel.x = std::signbit(m_vel.x)
                ? -Utillity::Player::maxSpeed
                : Utillity::Player::maxSpeed;
            }

        Move(_dT, world);
        if(m_vel.y > Utillity::epsilon)    {
            moves.set(Movment::fall);
        }

        std::swap(moves, lastMoves);
        MovmentFlags changes = (moves ^ lastMoves);
        moves = changes & lastMoves;
        // movment to animation
        if(moves.test(Movment::jump)) {
            m_animation.StartClip(Moves::jump);
            m_animation.QueueClip(Moves::down);
        } else if (moves.test(Movment::fall)) {
            m_animation.QueueClip(Moves::down);
        } else if (changes.test(Movment::fall)) {
            if(lastMoves.test(Movment::run)) {
                m_animation.StartClip(Moves::runR);
            } else {
                m_animation.QueueClip(Moves::idle);
            }
        } else if (changes.test(Movment::run)) {
            if(moves.test(Movment::run)) {
                m_animation.StartClip(Moves::runR);
            } else {
                m_animation.QueueClip(Moves::idle);
            }
        }

        m_mView = glm::translate(glm::mat4(1), glm::vec3(m_pos, 0));

    }

    const unsigned int* GetSpriteIdPtr() const { return &m_spriteId; }
    const bool* GetIsFlippedPtr() const { return &m_flippSprite; }
    const float* GetMViewPtr() const { return glm::value_ptr(m_mView);}
};