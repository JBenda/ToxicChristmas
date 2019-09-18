#pragma once

#include "Uttility.hpp"
#include "Object.hpp"
#include <memory>
#include <vector>

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
    float MoveH(const Collider& _coll, const glm::vec2& _target) const {
        glm::vec2 endPos = _coll.GetPos();
        bool movLR = _target.x > endPos.x;
        endPos.x = _target.x;
        
        m_objBuffer.resize(0);
        GetColliderInBound(m_level->statics, _coll, endPos, m_objBuffer);

        for(const Object* obj : m_objBuffer) {
                endPos.x = obj->EndPositionH(_coll, endPos);
        }
        return endPos.x;
    }
    float MoveV(const Collider& _coll, const glm::vec2& _target) const {
        glm::vec2 endPos = _coll.GetPos();
        endPos.y = _target.y;
        
        m_objBuffer.resize(0);
        GetColliderInBound(m_level->statics, _coll, endPos, m_objBuffer);
        std::cout << m_objBuffer.size() << '\n';
        for(const Object* obj : m_objBuffer) {
                endPos.y = obj->EndPositionV(_coll, endPos);
        }
        return endPos.y;
    }
private:
    std::shared_ptr<Level> m_level;
    mutable std::vector<const Object*> m_objBuffer;

    void GetColliderInBound (const std::vector<StaticObject*>& _orderList,
        const Collider& _coll,
        const glm::vec2& _target,
        std::vector<const Object*>& r_out) const {
        
        const glm::vec2 diff = 0.5f * (_target - _coll.GetPos());
        const glm::vec2 size = glm::vec2(fabs(diff.x), fabs(diff.y)) + _coll.GetRad();
        const Collider hitBox(size, _coll.GetPos() + diff);
        
        for(const StaticObject *obj : _orderList) {
            if(obj->IsIn(hitBox))
                r_out.push_back(obj);
        }
    }
 };