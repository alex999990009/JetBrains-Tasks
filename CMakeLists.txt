cmake_minimum_required(VERSION 3.0.0)
project(searcher VERSION 0.1.0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++17 -fPIC -lstdc++fs")

include(CTest)
enable_testing()

set(OPENGL_gl_LIBRARY /usr/lib/x86_64-linux-gnu/libGL.so)
set(OPENGL_INCLUDE_DIRS /usr/include/GL/)
set(SDL2_LIBRARY /usr/lib/x86_64-linux-gnu/libSDL2.so)
set(SDL2_INCLUDE_DIRS /usr/include/SDL2/)
set(GLM_INCLUDE_DIRS /usr/include/glm/)
set(GLU_LIBRARY /usr/lib/x86_64-linux-gnu/libGLU.so)
set(GLEW_LIBRARY /usr/lib/x86_64-linux-gnu/libGLEW.so.2.0.0)

set(IMGUI_SOURCES
    external_includes/imgui/imgui.cpp
    external_includes/imgui/imgui_draw.cpp
    external_includes/imgui/imgui_demo.cpp
    external_includes/imgui/imgui_tables.cpp
    external_includes/imgui/imgui_widgets.cpp
    external_includes/imgui/imgui.cpp
    external_includes/imgui/backends/imgui_impl_opengl3.cpp
    external_includes/imgui/backends/imgui_impl_sdl.cpp
   )

set(SOURCES
    src/main.cpp
    src/SearcherEngine.cpp
    src/SDLEnvironment.cpp
    src/OpenGLEnvironment.cpp
    )

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external_includes)
execute_process(
    COMMAND git clone "https://github.com/ocornut/imgui.git" imgui
    WORKING_DIRECTORY external_includes)
include_directories(external_includes/imgui)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external_includes)
execute_process(
    COMMAND git clone "https://github.com/AirGuanZ/imgui-filebrowser.git" imgui-filebrowser
    WORKING_DIRECTORY external_includes)
include_directories(external_includes/imgui-filebrowser)

add_executable(searcher ${SOURCES} ${IMGUI_SOURCES})

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/external_includes/imgui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/external_includes/imgui-filebrowser)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${OPENGL_INCLUDE_DIRS})

target_link_libraries(${CMAKE_PROJECT_NAME} ${SDL2_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} ${OPENGL_gl_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} ${GLU_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} ${GLEW_LIBRARY})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)