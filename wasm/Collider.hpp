#pragma once

#include "Uttility.hpp"

#include <glm/vec2.hpp>

#include <iostream>
class Collider {
protected:
    const glm::vec2 m_size;
    glm::vec2 m_pos;
    glm::vec2 m_vel;
    Collider(const glm::vec2& size, const glm::vec2& pos = glm::vec2(0)) : m_size{size}, m_pos{pos}, m_vel(0){}
    
    void Physics(float dT) {
        m_pos += m_vel * dT;
    }
};