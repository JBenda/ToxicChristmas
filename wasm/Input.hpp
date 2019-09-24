#pragma once

#include <cstring>

class Input{
public:
    static constexpr char Titels[] = "Up\0Down\0Left\0Right\0Jump\0";
    enum struct Inputs {Up, Down, Left, Right, Jump, LAST};
    struct InputState {
        unsigned char inputs[static_cast<unsigned int>(Inputs::LAST)];
        InputState(){}
        InputState(const InputState& state) {
            std::memcpy(inputs, state.inputs, static_cast<unsigned int>(Inputs::LAST));
        }
        InputState& operator=(const InputState& state)  {
            std::memcpy(inputs, state.inputs, static_cast<unsigned int>(Inputs::LAST));
            return *this;
        }
    };
    Input(const InputState& last, const InputState& now) : last{last}, now{now} {}
    bool KeyDown(Inputs key) const {
        return now.inputs[ static_cast<unsigned int>(key) ];
    }
    bool KeyUp(Inputs key) const {
        return now.inputs[ static_cast<unsigned int>(key) ];
    }
    bool KeyPressed(Inputs key) const {
        return now.inputs[ static_cast<unsigned int>(key) ] && !last.inputs[static_cast<unsigned int>(key)];
    }
    bool KeyReleased(Inputs key) const {
        return !now.inputs[ static_cast<unsigned int>(key) ] && last.inputs[ static_cast<unsigned int>(key) ];
    }
    unsigned char Value(Inputs axi) const { return now.inputs[ static_cast<unsigned int>(axi) ]; }
private:
    const InputState &last, &now;
};