#pragma once

#include <SDL2/SDL.h>

#include <string>

namespace searcher
{
    class SDLEnvironment
    {
    private:
        SDL_Window *window;
        SDL_GLContext gl_context;
        std::string glsl_version;

    public:
        SDLEnvironment();
        ~SDLEnvironment();

        SDL_Window *getWindow() const noexcept;
        SDL_GLContext getContext() const noexcept;
        std::string getGlslVersion() const noexcept;
    };
}
