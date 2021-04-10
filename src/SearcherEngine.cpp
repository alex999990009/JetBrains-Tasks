#include "SearcherEngine.hpp"
#include "SDLEnvironment.hpp"
#include "OpenGLEnvironment.hpp"

#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "imfilebrowser.h"

#include <iostream>
#include <fstream>

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

namespace searcher
{
    SearcherEngine::SearcherEngine()
    {
        sdlEnvPtr = std::make_unique<SDLEnvironment>();

        window = sdlEnvPtr->getWindow();
        gl_context = sdlEnvPtr->getContext();
        glsl_version = sdlEnvPtr->getGlslVersion();

        OpenGLEnvironment();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        io = std::make_unique<ImGuiIO>(ImGui::GetIO());

        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);

        ImFontConfig config;
        config.SizePixels = 24;
        io->Fonts->AddFontDefault(&config);
    }

    void SearcherEngine::search(std::filesystem::path &path, std::string &needle)
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
                    while (isalpha(line[leftPtr]))
                    {
                        --leftPtr;
                        if (leftPtr == 0)
                        {
                            break;
                        }
                    }
                    while (rightPtr < line.size() && isalpha(line[rightPtr]))
                    {
                        ++rightPtr;
                    }
                    words.push_back(line.substr(leftPtr + 1, rightPtr - leftPtr - 1));
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
            if (ImGui::Begin("customization"))
            {
                std::string title = !path.empty() ? path.string() : "open file dialog for download file";
                if (ImGui::Button(title.c_str()))
                {
                    fileDialog.Open();
                }

                ImGui::InputText("input needle", buf, MAX_STRING_SIZE);
                needle = buf;
                if (needle != oldNeedle)
                {
                    words.clear();
                }

                isSearching = ImGui::Button("search") | ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter));
                fileDialog.Display();

                if (fileDialog.HasSelected())
                {
                    path = fileDialog.GetSelected();
                    isFileLoaded = true;
                    fileDialog.ClearSelected();
                }
            }
            ImGui::End();

            ImGui::Begin("words");
            if (isFileLoaded && needle != oldNeedle && isSearching)
            {
                search(path, needle);
                oldNeedle = needle;
            }

            for (std::string word : words)
            {
                ImGui::Text(word.c_str());
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
