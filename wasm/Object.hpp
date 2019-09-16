#pragma once

#include "Uttility.hpp"
#include "Collider.hpp"
#include <glm/vec2.hpp>
#include <algorithm>

class Object : public Collider {
protected:
    Object(const glm::vec2& size, const glm::vec2& pos)
    : Collider(size, pos){}
public:
    virtual bool CollideH(const Collider& coll, const glm::vec2& target) const = 0;
    virtual float EndPositionH(const Collider& _coll, const glm::vec2& target) const = 0;
};

class StaticObject : public Object {
private:
    static constexpr float w = Utillity::StaticObjectSize, h = Utillity::StaticObjectSize;
protected:
    StaticObject(const glm::vec2& pos)
    : Object(glm::vec2(w, h), pos){}
public:
    virtual ~StaticObject() = default;
    unsigned int spriteId;
    float frameTime, frames, offset;
};

class Box : public StaticObject {
public:
    Box(const glm::vec2& pos)
    : StaticObject(pos) {
        spriteId = 0;
        frameTime = 0.1f;
        frames = 4;
        offset = 0;
    }
    bool CollideH(const Collider& coll, const glm::vec2& target) const override {
        return abs(target.x - m_pos.x) >= m_size.x;
    }
    float EndPositionH(const Collider& coll, const glm::vec2& target) const override {
        if(target.x - coll.GetPos().x > 0) 
            return std::min(target.x, m_pos.x - m_size.x);
        else
            return std::min(target.x, m_pos.x + m_size.x);
    }
};