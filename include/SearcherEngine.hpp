#pragma once

#include "imgui.h"
#include "SDLEnvironment.hpp"
#include "OpenGLEnvironment.hpp"
#include <SDL.h>

class SearcherEngine
{
private:
    SDLEnvironment *sdl;
    OpenGLEnvironment *openGL;
    ImGuiIO *io;

    SDL_Window *window;
    SDL_GLContext gl_context;
    const char *glsl_version;

    // ImGuiIO io;
public:
    SearcherEngine(/* args */);
    ~SearcherEngine();

    void run();
};
