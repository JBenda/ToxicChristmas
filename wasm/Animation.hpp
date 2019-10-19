#pragma once

#include "Uttility.hpp"

#ifdef DEBUG
#include <cstring>
#include <iostream>
#include <queue>
#endif

template<typename T>
class Animation {
    struct Clip{
        Clip(const unsigned int* frames, float frameTime, unsigned int length) : 
            frameTime{frameTime},
            frames{frames}, 
            length{length} {}
        float frameTime;
        const unsigned int* frames;
        unsigned int length;
        Clip() : frameTime{0}, frames{nullptr}, length{0} {}
    } m_clips[ static_cast<unsigned int>(T::LAST) ];
#ifdef DEBUG
    bool m_active[ static_cast<unsigned int>(T::LAST) + 1 ];
#endif
    float timer;
    unsigned int m_activAnimation;
    unsigned int m_frame;
    std::queue<T> m_queue;
public:
    Animation(T startAnimation = static_cast<T>(0))
        :   m_activAnimation{static_cast<unsigned int>(startAnimation)},
            timer{0},
            m_frame{0},
            m_queue{}{
                #ifdef DEBUG
                    std::memset(m_active, 0, sizeof(m_active));
                #endif
            }

    void DefineClip(T animation, const unsigned int* frames, unsigned int length, float frameTime) {
        Clip& c = m_clips[ static_cast<unsigned int>(animation) ];
        c.frames = frames;
        c.length = length;
        c.frameTime = frameTime;
    #ifdef DEBUG
        m_active[static_cast<unsigned int>(animation) + 1] = true;
        m_active[0] = true;
        for(int i = 1; i <= static_cast<unsigned int>(T::LAST); ++i) {
            if(!m_active[i]) {
                m_active[0] = false;
            }
        }
    #endif
    }

    void Update(float dt) {
    #ifdef DEBUG
        if (!m_active[0]) throw "not All Clips set";
    #endif
        timer -= dt;
        while (timer <= 0) {
            if(++m_frame == m_clips[m_activAnimation].length) {
                if(!m_queue.empty()) {
                    m_activAnimation = static_cast<unsigned int>(m_queue.front());
                    m_queue.pop();
                }
                m_frame = 0;
            }

            timer += m_clips[m_activAnimation].frameTime;
        }
    }

    unsigned int GetSpireId() {
        return m_clips[m_activAnimation].frames[m_frame];
    
    }

    void StartClip(T clip) {
        std::queue<T>{}.swap(m_queue); // clrear Queue TODO: better
        m_activAnimation = static_cast<unsigned int>(clip);
        m_frame = 0;
        timer = m_clips[m_activAnimation].frameTime;
    }

    void QueueClip(T clip) {
        m_queue.push(clip);
    }
};