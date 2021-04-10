#pragma once

#include "imgui.h"
#include "SDLEnvironment.hpp"
#include "OpenGLEnvironment.hpp"
#include <SDL2/SDL.h>

#include <filesystem>
#include <vector>

namespace searcher
{
    class SearcherEngine
    {
    private:
        std::unique_ptr<SDLEnvironment> sdlEnvPtr;
        std::unique_ptr<ImGuiIO> io;

        SDL_Window *window;
        SDL_GLContext gl_context;
        const char *glsl_version;

        std::vector<std::string> words;

    public:
        SearcherEngine();
        ~SearcherEngine();

        void search(std::filesystem::path &path, std::string &str);

        void run();
    };
}
