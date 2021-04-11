#include "SearcherEngine.hpp"
#include "OpenGLEnvironment.hpp"

#include <imgui.h>
#include <backends/imgui_impl_sdl.h>
#include <backends/imgui_impl_opengl3.h>
#include <imfilebrowser.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE
#include <glbinding/Binding.h>
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE
#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

#include <iostream>
#include <fstream>
#include <iterator>

namespace searcher
{
    SearcherEngine::SearcherEngine()
    {
        sdlEnv = std::make_unique<SDLEnvironment>();

        OpenGLEnvironment();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = std::make_unique<ImGuiIO>(ImGui::GetIO());

        ImGui::StyleColorsLight();

        ImGui_ImplSDL2_InitForOpenGL(sdlEnv->getWindow(), sdlEnv->getContext());
        ImGui_ImplOpenGL3_Init(sdlEnv->getGlslVersion().c_str());

        ImFontConfig config;
        config.SizePixels = 24;
        io->Fonts->AddFontDefault(&config);
    }

    void SearcherEngine::printWord(const std::string &word, const std::vector<std::size_t> &positions) const noexcept
    {
        std::size_t lastIndex = 0;
        for (std::size_t position : positions)
        {
            ImGui::Text(word.substr(lastIndex, position - lastIndex).c_str());
            ImGui::SameLine(0, 0);
            ImGui::TextColored(ImVec4(255, 0, 0, 1), std::string(1, word[position]).c_str());
            ImGui::SameLine(0, 0);
            lastIndex = position + 1;
        }
        ImGui::Text(word.substr(lastIndex, word.size() - lastIndex).c_str());
    }

    bool SearcherEngine::checkSubstring(const std::string &word, const std::string &needle) const noexcept
    {
        std::size_t position = word.find(needle);
        if (position != std::string::npos)
        {
            std::vector<std::size_t> positions(needle.size());
            for (std::size_t i = position; i < position + needle.size(); ++i)
            {
                positions[i - position] = i;
            }
            printWord(word, positions);
            return true;
        }
        return false;
    }

    bool SearcherEngine::checkSubsequence(const std::string &word, const std::string &needle) const noexcept
    {
        std::vector<std::size_t> positions(needle.size());
        std::size_t ptrNeedle = 0;
        for (std::size_t i = 0; i < word.size(); ++i)
        {
            if (needle[ptrNeedle] == word[i])
            {
                positions[ptrNeedle++] = i;
            }
            if (ptrNeedle == needle.size())
            {
                break;
            }
        }
        if (ptrNeedle == needle.size())
        {
            printWord(word, positions);
            return true;
        }
        return false;
    }

    void SearcherEngine::search(const std::filesystem::path &path, const std::string &needle, bool isSubstring)
    {
        foundWords.clear();

        std::ifstream file;
        file.open(path.string());

        std::string line;
        while (std::getline(file, line))
        {
            std::stringstream ss(line);

            std::vector<std::string> tokens;
            std::copy(std::istream_iterator<std::string>(ss),
                      std::istream_iterator<std::string>{},
                      std::back_inserter(tokens));

            for (std::string &word : tokens)
            {
                if ((isSubstring && checkSubstring(word, needle)) ||
                    (!isSubstring && checkSubsequence(word, needle)))
                {
                    foundWords.push_back(word);
                }
            }
        }
    }

    void SearcherEngine::run()
    {
        const int MAX_STRING_SIZE = 255;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui::FileBrowser fileDialog;
        bool isFileLoaded = false;
        std::filesystem::path path;

        std::string oldNeedle;
        std::string needle;
        char buf[MAX_STRING_SIZE] = {0};
        bool isSubstring = true;

        bool done = false;
        while (!done)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                {
                    done = true;
                }
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                    event.window.windowID == SDL_GetWindowID(sdlEnv->getWindow()))
                {
                    done = true;
                }
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(sdlEnv->getWindow());
            ImGui::NewFrame();

            bool isSearching = false;
            int width, height;
            SDL_GetWindowSize(sdlEnv->getWindow(), &width, &height);
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(width + 2, height + 2), ImGuiCond_Always);

            if (ImGui::Begin("window searcher"))
            {
                std::string title = !path.empty() ? path.string() : "Choose file for searching words";
                if (ImGui::Button(title.c_str()))
                {
                    fileDialog.Open();
                }

                ImGui::Dummy(ImVec2(0, 20));

                ImGui::Text("Choose the mode");
                bool changeMode = false;

                if (ImGui::RadioButton("substring", isSubstring))
                {
                    changeMode = true;
                    isSubstring = true;
                }

                if (ImGui::RadioButton("subsequence", !isSubstring))
                {
                    changeMode = true;
                    isSubstring = false;
                }

                ImGui::Dummy(ImVec2(0, 20));

                ImGui::Text("Type needle and press Enter or button \"search\"");
                ImGui::InputText("", buf, MAX_STRING_SIZE);
                needle = buf;
                if (needle != oldNeedle)
                {
                    foundWords.clear();
                    oldNeedle = needle;
                }
                ImGui::SameLine();
                isSearching = ImGui::Button("search") |
                              ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter)) |
                              changeMode;

                fileDialog.Display();

                if (fileDialog.HasSelected())
                {
                    path = fileDialog.GetSelected();
                    isFileLoaded = true;
                    fileDialog.ClearSelected();
                }
            }
            ImGui::Dummy(ImVec2(0, 20));

            ImGui::Text("Found words:");
            ImGui::Text("------------");

            if (isFileLoaded && isSearching)
            {
                search(path, needle, isSubstring);
            }
            else
            {
                for (std::string &word : foundWords)
                {
                    if (isSubstring)
                    {
                        checkSubstring(word, needle);
                    }
                    else
                    {
                        checkSubsequence(word, needle);
                    }
                }
            }
            ImGui::End();

            // Rendering
            ImGui::Render();
            glViewport(0, 0, io->DisplaySize.x, io->DisplaySize.y);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w,
                         clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(sdlEnv->getWindow());
        }
    }

    SearcherEngine::~SearcherEngine()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}
