cmake_minimum_required(VERSION 3.10)

project(GraphScript-All VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(LIBRARIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty" CACHE INTERNAL "")

# SDL
add_subdirectory(submodules/sdl)

# imgui
set(IMGUI_DIR "${LIBRARIES_DIR}/imgui" CACHE INTERNAL "")

# graphscript
set(GRAPHSCRIPT_DIR "${LIBRARIES_DIR}/imgui" CACHE INTERNAL "")
add_subdirectory(graphscript)
add_subdirectory(submodules/raylib)
add_subdirectory(graphscript-raylib)

add_subdirectory(example)
add_subdirectory(graphscript-runtime-shared)
add_subdirectory(sandbox)
add_subdirectory(runtime)