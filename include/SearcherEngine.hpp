#pragma once

#include "SDLEnvironment.hpp"

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
        std::vector<std::string> foundWords;

        void printWord(const std::string &word, const std::vector<std::size_t> &positions) const noexcept;

        bool checkSubstring(const std::string &word, const std::string &needle) const noexcept;

        bool checkSubsequence(const std::string &word, const std::string &needle) const noexcept;

        void search(const std::filesystem::path &path, const std::string &needle, bool isSubstring);

    public:
        SearcherEngine();
        ~SearcherEngine();

        void run();
    };
}
