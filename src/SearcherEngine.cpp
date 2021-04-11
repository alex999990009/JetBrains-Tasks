#include "SearcherEngine.hpp"
#include "SDLEnvironment.hpp"
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

namespace searcher
{
    SearcherEngine::SearcherEngine()
    {
        sdlEnv = std::make_unique<SDLEnvironment>();

        window = sdlEnv->getWindow();
        gl_context = sdlEnv->getContext();

        OpenGLEnvironment();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = std::make_unique<ImGuiIO>(ImGui::GetIO());

        ImGui::StyleColorsLight();

        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init(sdlEnv->getGlslVersion().c_str());

        ImFontConfig config;
        config.SizePixels = 24;
        io->Fonts->AddFontDefault(&config);
    }

    bool SearcherEngine::checkSymbol(const char c) const noexcept
    {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\a')
        {
            return true;
        }
        return false;
    }

    void SearcherEngine::printWord(const std::string &word, const std::size_t position, const std::size_t lenSubStr) const noexcept
    {
        ImGui::Text(word.substr(0, position).c_str());
        ImGui::SameLine(0, 0);
        ImGui::TextColored(ImVec4(255, 0, 0, 1), word.substr(position, lenSubStr).c_str());
        ImGui::SameLine(0, 0);
        ImGui::Text(word.substr(position + lenSubStr, word.size() - (position + lenSubStr)).c_str());
    }

    void SearcherEngine::search(const std::filesystem::path &path, const std::string &needle)
    {
        std::ifstream file;
        file.open(path.string());

        std::string line;
        while (std::getline(file, line))
        {
            std::size_t position = -1;
            while (true)
            {
                position = line.find(needle, position + 1);
                if (position != std::string::npos)
                {
                    std::size_t leftPtr = position;
                    std::size_t rightPtr = position;
                    while (!checkSymbol(line[leftPtr]))
                    {
                        if (leftPtr == 0)
                        {
                            leftPtr = -1;
                            break;
                        }
                        --leftPtr;
                    }
                    while (rightPtr < line.size() && !checkSymbol(line[rightPtr]))
                    {
                        ++rightPtr;
                    }

                    std::string word = line.substr(leftPtr + 1, rightPtr - leftPtr - 1);
                    printWord(word, position - leftPtr - 1, needle.size());

                    words.push_back(std::move(word));
                }
                else
                {
                    break;
                }
            }
        }
    }

    void SearcherEngine::run()
    {
        const int MAX_STRING_SIZE = 255;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        ImGui::FileBrowser fileDialog;
        fileDialog.SetTitle("title");

        bool done = false;
        bool isFileLoaded = false;
        std::filesystem::path path;
        std::string oldNeedle;
        std::string needle;
        char buf[MAX_STRING_SIZE] = {0};
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
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                {
                    done = true;
                }
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            bool isSearching = false;
            int width, height;
            SDL_GetWindowSize(window, &width, &height);
            ImGui::SetNextWindowPos(ImVec2(.0f, .0f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(width + 2, height + 2), ImGuiCond_Always);
            if (ImGui::Begin("window searcher"))
            {
                std::string title = !path.empty() ? path.string() : "choose file for searching words";
                if (ImGui::Button(title.c_str()))
                {
                    fileDialog.Open();
                }

                ImGui::Dummy(ImVec2(0.0f, 20.0f));
                ImGui::Text("type needle and press Enter or button \"search\"");

                ImGui::InputText("", buf, MAX_STRING_SIZE);
                needle = buf;
                if (needle != oldNeedle)
                {
                    words.clear();
                    oldNeedle = needle;
                }

                ImGui::SameLine();
                isSearching = ImGui::Button("search") | ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter));
                fileDialog.Display();

                if (fileDialog.HasSelected())
                {
                    path = fileDialog.GetSelected();
                    isFileLoaded = true;
                    fileDialog.ClearSelected();
                }
            }

            ImGui::Dummy(ImVec2(0.0f, 20.0f));
            ImGui::Text("found words");
            ImGui::Text("-----------");

            if (isFileLoaded && isSearching)
            {
                words.clear();
                search(path, needle);
            }
            else
            {
                for (std::string word : words)
                {
                    printWord(word, word.find(needle), needle.size());
                }
            }
            ImGui::End();

            // Rendering
            ImGui::Render();
            glViewport(0, 0, io->DisplaySize.x, io->DisplaySize.y);
            glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);
        }
    }

    SearcherEngine::~SearcherEngine()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }
}
