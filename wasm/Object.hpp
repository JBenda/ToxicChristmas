#pragma once

#include "Uttility.hpp"
#include "Collider.hpp"
#include <glm/vec2.hpp>
#include <algorithm>
#include <optional>

class Object : public Collider {
protected:
    Object(const glm::vec2& size, const glm::vec2& pos)
    : Collider(size, pos){}
public:
    virtual float EndPositionH(const Collider& _coll, const glm::vec2& _target) const = 0;
    virtual float EndPositionV(const Collider& _coll, const glm::vec2& _target) const = 0;
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
    float EndPositionH(const Collider& _coll, const glm::vec2& _target) const override {
        float diff = _target.x - _coll.GetPos().x;
        if (diff == 0) return _target.x;

        bool movLR = diff > 0;
        float end = m_pos.x + (movLR ? -1 : 1) * (m_size.x + _coll.GetRad().x + Utillity::epsilon);
        if(movLR) {
            return std::min(end, _target.x);
        }  else {
            return std::max(end, _target.x);
        }         
    }
    float EndPositionV(const Collider& _coll, const glm::vec2& _target) const override {
        float diff = _target.y - _coll.GetPos().y;
        if (diff == 0) return _target.y;

        bool movUD = diff > 0;
        float end = m_pos.y + (movUD ? -1 : 1) * (m_size.y + _coll.GetRad().y + Utillity::epsilon);
        if(movUD) {
            return std::min(end, _target.y);
        } else {
            return std::max(end, _target.y);
        }
    }
};