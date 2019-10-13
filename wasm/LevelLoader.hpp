#pragma once

#include "Uttility.hpp"

#include "Object.hpp"

class LevelLoader
{
public:
    enum struct Tiles { Box, ClessBox, HBox, Rising, Falling, LAST};
    static constexpr unsigned char ColorEncoding[] = {0, 10, 20, 100, 200}; ///< red value = tile type
    template<typename T>
    static const T* GetTemplate(Tiles _tile) {
        if(!m_templates)
            InitelizeTemplates();
#ifdef DEBUG
        return dynamic_cast<T*>(m_templates[static_cast<unsigned int>(_tile)]);
#elif
        return reinterpret_cast<T*>(m_templates[static_cast<unsigned int>(_tiles)]);
#endif
    }
    static StaticObject* Instanciate(Tiles _tile, const glm::vec2& _pos) {
        if(!m_templates)
            InitelizeTemplates();
        return m_templates[static_cast<unsigned int>(_tile)]->Instanciate(_pos); // TODO add offset
    }
private:
    static StaticObject** m_templates;
    static void InitelizeTemplates() {
        m_templates = new StaticObject*[static_cast<unsigned int>(Tiles::LAST)];

        Box* box = new Box({0,0});
        box->frameTime = 0.1f;
        box->frames = 1;
        box->spriteId = 0;
        m_templates[static_cast<unsigned int>(Tiles::Box)] = box;

        Img* clessBox = new Img({0,0});
        clessBox->frameTime = 0.1f;
        clessBox->frames = 1;
        clessBox->spriteId = 0;
        m_templates[static_cast<unsigned int>(Tiles::ClessBox)] = clessBox;

        HBox* hbox = new HBox({0,0});
        hbox->frameTime = 0.1f;
        hbox->frames = 1;
        hbox->spriteId = 0;
        m_templates[static_cast<unsigned int>(Tiles::HBox)] = hbox;

        Slop* rising = new Slop({0,0}, -2, 0);
        rising->frameTime = 0.1f;
        rising->frames = 1;
        rising->spriteId = 1;
        m_templates[static_cast<unsigned int>(Tiles::Rising)] = rising;

        Slop* falling = new Slop({0,0}, 0,-2);
        falling->frameTime = 0.1f;
        falling->frames = 1;
        falling->spriteId = 2;
        m_templates[static_cast<unsigned int>(Tiles::Falling)] = falling;
        
    }
};
