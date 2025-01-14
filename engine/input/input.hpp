#pragma once
#include "core/common.hpp"
#include <unordered_map>

class Input {
    public:
        Input() {}
        ~Input() {}

        void update(SDL_Event& event, bool& quit);
        bool isKeyPressed(SDL_Scancode scancode) const;
        bool isMouseButtonPressed(uint8_t button) const;
    private:
        std::unordered_map<SDL_Scancode, bool> keys;
        std::unordered_map<uint8_t, bool> mouseButton;

};