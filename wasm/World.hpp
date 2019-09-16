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
    }
private:
    std::shared_ptr<Level> m_level;
 };