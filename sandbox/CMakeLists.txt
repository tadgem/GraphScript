cmake_minimum_required(VERSION 3.17)
project(GraphScript-Sandbox)

set(CMAKE_CXX_STANDARD 17)
add_executable(GraphScript-Sandbox
        "${LIBRARIES_DIR}/imgui/src/imgui_impl_sdl3.cpp"
        "${LIBRARIES_DIR}/imgui/src/imgui_impl_sdlrenderer3.cpp"
        "src/main.cpp"
        "src/App.cpp"
        "src/GraphScriptSandbox.cpp"
        "src/imgui_filedialog.cpp"
        "${IMGUI_DIR}/src/imgui.cpp"
        "${IMGUI_DIR}/src/imgui_draw.cpp"
        "${IMGUI_DIR}/src/imgui_demo.cpp"
        "${IMGUI_DIR}/src/imgui_tables.cpp"
        "${IMGUI_DIR}/src/imgui_widgets.cpp"
        "${IMGUI_DIR}/src/imnodes.cpp"
        "include/GraphScriptSandbox.h"
        "include/imgui_filedialog.h"
        "include/App.h"
        )


get_filename_component(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)


# SDL
target_link_libraries(${PROJECT_NAME} SDL3-static)
target_include_directories(${PROJECT_NAME} PRIVATE "${ROOT_DIR}/submodules/sdl/include")

# imgui
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}/include")

# graphscript-raylib
target_link_libraries(${PROJECT_NAME} GraphScript GraphScript-Raylib GraphScript-Runtime-Shared)
target_include_directories(${PROJECT_NAME} PRIVATE "${ROOT_DIR}/graphscript/include" "include" ${GRAPHSCRIPT_RAYLIB_HEADERS} ${GRAPHSCRIPT_RUNTIME_SHARED_HEADERS})

