#pragma once

#include "Uttility.hpp"
#include "Object.hpp"
#include <memory>
#include <vector>
#include <optional>

struct Level {
    Level(const glm::vec2& _start, const Utillity::Rect& _target) : start{_start}, target{_target}{}
    ~Level() {
        for(StaticObject* o : statics) {
            delete o;
        }
    }
    std::vector<StaticObject*> statics;
    glm::vec2 start;
    Utillity::Rect target;
};

class World{
public:
    const std::vector<StaticObject*>& GetStaticObjects() { return m_level->statics; }
    void LoadLevel(const std::shared_ptr<Level>& _level) {
        m_level = _level;
        unsigned int size = m_level->statics.size();
    }
    std::optional<float> MoveH(const Collider& _coll, const glm::vec2& _target) const {
        bool movR = _target.x > _coll.GetPos().x;
        m_objBuffer.resize(0);
        GetCrossedObj(m_level->statics, _coll, _target, m_objBuffer);
        if(movR)
            sort(m_objBuffer.begin(), m_objBuffer.end(), Collider::SortPosHorizontel());
        else
            sort(m_objBuffer.begin(), m_objBuffer.end(), Utillity::OrderReverser(Collider::SortPosHorizontel()));
        for(const Object *o : m_objBuffer) {
            float x = o->EndPositionH(_coll, _target, movR);
            if(x != _target.x)
                return std::optional(x);
        }
        return std::optional<float>();
    }

    std::optional<float> MoveV(const Collider& _coll, const glm::vec2& _target) const {
        bool movD = _target.y > _coll.GetPos().y;
        m_objBuffer.resize(0);
        GetCrossedObj(m_level->statics, _coll, _target, m_objBuffer);
        if(movD)
            sort(m_objBuffer.begin(), m_objBuffer.end(), Collider::SortPosVertical());
        else
            sort(m_objBuffer.begin(), m_objBuffer.end(), Utillity::OrderReverser(Collider::SortPosVertical()));
        
        for(const Object* o : m_objBuffer) {
            float y = o->EndPositionV(_coll, _target, movD);
            if(y != _target.y) {
                return std::optional(y);
            }
        }
        return std::optional<float>();
    }
private:
    std::shared_ptr<Level> m_level;
    mutable std::vector<const Object*> m_objBuffer;

    void GetCrossedObj(const std::vector<StaticObject*>& _list, const Collider& _coll, const glm::vec2& _target, std::vector<const Object*>& r_res) const {
        const glm::vec2 mov = _target - _coll.GetPos();
        const glm::vec2 mid = (_target + _coll.GetPos()) * 0.5f;
        Collider hitBox(_coll.GetRad(), _target);
        for(StaticObject* o : _list) {
            if(o->IsIn(hitBox))
                r_res.push_back(o);
        }
    }
 };