#pragma once

#include <SDL2/SDL.h>

#include <memory>

namespace searcher
{
    class SDLEnvironment
    {
    private:
        SDL_WindowFlags window_flags;
        SDL_Window *window;
        SDL_GLContext gl_context;
        const char *glsl_version;

    public:
        SDLEnvironment();
        ~SDLEnvironment();

        SDL_Window *getWindow();
        SDL_GLContext getContext();
        const char *getGlslVersion();
    };
}
