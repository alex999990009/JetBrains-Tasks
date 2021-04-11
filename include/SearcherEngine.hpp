#pragma once

#include "SDLEnvironment.hpp"
#include "OpenGLEnvironment.hpp"

#include <imgui.h>
#include <SDL2/SDL.h>

#include <filesystem>
#include <vector>

namespace searcher
{
    class SearcherEngine
    {
    private:
        std::unique_ptr<SDLEnvironment> sdlEnv;
        std::unique_ptr<ImGuiIO> io;

        SDL_Window *window;
        SDL_GLContext gl_context;

        std::vector<std::string> words;

        void search(const std::filesystem::path &path, const std::string &str);

        bool checkSymbol(const char c) const noexcept;

        void printWord(const std::string &word, const std::size_t position, const std::size_t lenSubStr) const noexcept;

    public:
        SearcherEngine();
        ~SearcherEngine();

        void run();
    };
}
