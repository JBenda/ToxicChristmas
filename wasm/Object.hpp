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
    virtual float EndPositionH(const Collider& _coll, const glm::vec2& _target, bool movR) const = 0;
    virtual float EndPositionV(const Collider& _coll, const glm::vec2& _target, bool movD) const = 0;
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
    virtual StaticObject* Instanciate(const glm::vec2& _pos, float offset = 0) const = 0;
};

class Box : public StaticObject {
public:
    Box(const glm::vec2& pos, unsigned int sid = 0)
    : StaticObject(pos) {
        spriteId = sid;
        frameTime = 0.1f;
        frames = 1;
        offset = 0;
    }
    StaticObject* Instanciate(const glm::vec2& _pos, float _offset) const override {
        Box* copy = new Box(_pos, spriteId);
        copy->frameTime = frameTime;
        copy->frames = frames;
        copy->offset = _offset;
        return copy;
    }
    float EndPositionH(const Collider& _coll, const glm::vec2& _target, bool movR) const override {
        if(movR) {
            return std::min(
                _target.x,
                m_pos.x - m_size.x - _coll.GetRad().x - Utillity::epsilon
            );
        } else {
            return std::max(
                _target.x,
                m_pos.x + m_size.x + _coll.GetRad().x + Utillity::epsilon
            );
        }
    }
    float EndPositionV(const Collider& _coll, const glm::vec2& _target, bool movD) const override {
         if(movD) {
             return std::min(
                 _target.y,
                 m_pos.y - m_size.y - _coll.GetRad().y - Utillity::epsilon
             );
         } else {
             return std::max(
                 _target.y,
                 m_pos.y + m_size.y + _coll.GetRad().y + Utillity::epsilon
             );
         }
    }
};

class Slop : public Box {
    float m_oLeft, m_oRight;
    bool m_rising; /**<< from left to right slop is rising */
public:
    Slop(const glm::vec2& _pos, float _oLeft, float _oRight)
        : Box(_pos), m_oLeft{_oLeft}, m_oRight{_oRight}, m_rising{_oLeft > _oRight}
        {}
    StaticObject* Instanciate(const glm::vec2& _pos, float _offset) const override{
        Slop* copy = new Slop(_pos, m_oLeft, m_oRight);
        copy->spriteId = spriteId;
        copy->frameTime = frameTime;
        copy->frames = frames;
        copy->offset = _offset;
        return copy;
    }

    float EndPositionH(const Collider& _coll, const glm::vec2& _target, bool movR) const override {
        if(movR == m_rising && !_coll.IsIn(*this)) {
            return Box::EndPositionH(_coll, _target, movR);
        } else {
            return _target.x;
        }
    }
    float EndPositionV(const Collider& _coll, const glm::vec2& _target, bool movD) const override {
        if(movD) {
            float relSlopPos = (_target.x 
                + (m_rising ? -_coll.GetRad().x  : _coll.GetRad().x)- m_pos.x) / m_size.x * 0.5f + 0.5f;
            relSlopPos = std::clamp<float>(relSlopPos, 0, 1);
            float relSlopPos_1 = 1.0f - relSlopPos;
#ifdef DEBUG
            assert(relSlopPos + relSlopPos_1 <= 1.0f);
#endif
            const float offset = relSlopPos_1 * m_oLeft + relSlopPos  * m_oRight;
            return std::min(
                _target.y,
                m_pos.y - offset - m_size.y - _coll.GetRad().y - Utillity::epsilon
            );
        } else {
            if(!_coll.IsIn(*this))
                return Box::EndPositionV(_coll, _target, movD);
            else 
                return _target.y;
        }
    }
};