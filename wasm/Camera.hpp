#pragma once

#include <cmath>

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "World.hpp"
#include "Player.hpp"
#include "Input.hpp"

class Camera {
public:
    Camera(const glm::vec2& _bottomLeft, const glm::vec2& _topRight, glm::vec2 _pos = glm::vec2(0,0)) : m_pos{_pos, 0}, m_area_2(fabs(_bottomLeft.x - _topRight.x) * 0.5f, fabs(_bottomLeft.y - _topRight.y)  *0.5f){
        m_projection = glm::ortho(_bottomLeft.x, _topRight.x, _bottomLeft.y, _topRight.y, -1.0f, 1.0f);
        m_projectionMoved = glm::translate(m_projection, m_pos);
    }
    const float* GetProjectionMatrix() const { return glm::value_ptr(m_projectionMoved); }
    void SetPosition(const glm::vec2& _pos) {
        m_pos = glm::vec3(-_pos, 0);
        m_projectionMoved = glm::translate(m_projection, m_pos);
    }
    void Update(float _dT, const Input& _input, const Player& _player, const World& _world) {
        glm::vec2 origin;
        unsigned int w, h;
        _world.GetDimensions(w, h, origin);
        glm::vec2 pos = _player.GetPos() - origin;

        if (pos.x + m_area_2.x > static_cast<float>(w)) {
            pos.x = static_cast<float>(w) - m_area_2.x;
        } else if (pos.x - m_area_2.x < 0.f) {
            pos.x = m_area_2.x;
        }

        if (pos.y + m_area_2.y > static_cast<float>(h)) {
            pos.y = static_cast<float>(h) - m_area_2.y;
        } else if (pos.y - m_area_2.y < 0.f) {
            pos.y = m_area_2.y;
        }
        SetPosition(pos + origin);
    }
private:
    glm::mat4 m_projectionMoved; /**< projection Matrix moved to Position*/
    glm::mat4 m_projection; /**< projction Matrix */
    glm::vec3 m_pos;
    glm::vec2 m_area_2;
};
