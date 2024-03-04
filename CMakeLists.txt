cmake_minimum_required(VERSION 3.10)

project(GraphScript-All VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(LIBRARIES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty" CACHE INTERNAL "")

# glad
set(GLAD_DIR "${LIBRARIES_DIR}/glad" CACHE INTERNAL "")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")


# glfw
set(GLFW_DIR "${LIBRARIES_DIR}/glfw" CACHE INTERNAL "")
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "Build the GLFW example programs")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "Build the GLFW test programs")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "Build the GLFW documentation")
set(GLFW_INSTALL OFF CACHE INTERNAL "Generate installation target")
# add_subdirectory("${GLFW_DIR}")

# imgui
set(IMGUI_DIR "${LIBRARIES_DIR}/imgui" CACHE INTERNAL "")
add_library("imgui"
        "${IMGUI_DIR}/src/imgui.cpp"
        "${IMGUI_DIR}/src/imgui_draw.cpp"
        "${IMGUI_DIR}/src/imgui_demo.cpp"
        "${IMGUI_DIR}/src/imgui_widgets.cpp"
        "${IMGUI_DIR}/src/imnodes.cpp")
target_include_directories("imgui" PRIVATE "${IMGUI_DIR}/include")

# graphscript
set(GRAPHSCRIPT_DIR "${LIBRARIES_DIR}/imgui" CACHE INTERNAL "")
add_subdirectory(graphscript)
add_subdirectory(submodules/raylib)
add_subdirectory(graphscript-raylib)

add_subdirectory(example)
add_subdirectory(graphscript-runtime-shared)
add_subdirectory(sandbox)
add_subdirectory(runtime)