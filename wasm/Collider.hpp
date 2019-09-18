#pragma once

#include "Uttility.hpp"

#include <cmath>
#include <glm/vec2.hpp>
#include <iostream>

class Collider {
protected:
    const glm::vec2 m_size;
    glm::vec2 m_pos;
    glm::vec2 m_vel;

public:
    Collider(const glm::vec2& size, const glm::vec2& pos = glm::vec2(0)) : m_size{size}, m_pos{pos}, m_vel(0){}
    
    const glm::vec2& GetPos() const { return m_pos; }
    const glm::vec2& GetVel() const { return m_vel; }
    const glm::vec2 GetRad() const { return m_size;}
    bool IsIn(const Collider& _coll) const {
        return IsIn(_coll, _coll.m_pos);
    }
    bool IsIn(const Collider& _coll, const glm::vec2& _pos) const {
        glm::vec2 diff = m_pos - _pos;
        diff.x = fabs(diff.x);
        diff.y = fabs(diff.y);
        if(diff.x > diff.y) {
            return diff.x <= (m_size.x + _coll.m_size.x);
        } else {
            return diff.y <= (m_size.y + _coll.m_size.y);
        }
    }
    static struct  {
        bool operator()(const Collider *a, const Collider *b) const {
            return a->m_pos.y < b->m_pos.y;
        }
    } sortPosVertical;
    static struct {
       bool operator()(const Collider *a, const Collider *b) const {
           return a->m_pos.x < b->m_pos.x;
       } 
    } sortPosHorizontel;
};