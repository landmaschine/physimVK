#include "input.hpp"
#include "renderer/rendererVK.hpp"

void Input::update(SDL_Event& event, bool& quit, RendererVK& rendererVK) {
    int new_width = 0;
    int new_height = 0;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_EVENT_KEY_DOWN:
                keys[event.key.scancode] = true;
                break;
            case SDL_EVENT_KEY_UP:
                keys[event.key.scancode] = false;
                break;
            case SDL_EVENT_QUIT:
                quit = true;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                new_width = event.window.data1;
                new_height = event.window.data2;
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                mouseButton[event.button.button] = true;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                mouseButton[event.button.button] = false;
                break;
        }
        
        ImGui_ImplSDL3_ProcessEvent(&event);

        if(event.type == SDL_EVENT_WINDOW_RESIZED) {
            int width, height;
            SDL_GetWindowSizeInPixels(rendererVK._window, &width, &height);
            rendererVK.resize(width, height);
        }
    }
}

bool Input::isKeyPressed(SDL_Scancode scancode) const {
    const bool* keyState = SDL_GetKeyboardState(nullptr);
    return keyState[scancode];
}

bool Input::isMouseButtonPressed(uint8_t button) const {
    auto it = mouseButton.find(button);
    return it != mouseButton.end() && it->second;
}