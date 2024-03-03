cmake_minimum_required(VERSION 3.17)
project(GraphScript-Sandbox)

set(CMAKE_CXX_STANDARD 17)
add_executable(GraphScript-Sandbox
        "${LIBRARIES_DIR}/imgui/src/imgui_impl_glfw.cpp"
        "${LIBRARIES_DIR}/imgui/src/imgui_impl_opengl3.cpp"
        "src/main.cpp"
        "src/App.cpp"
        "src/Utils.cpp"
        "src/GraphScriptEditor.cpp"
        "src/imgui_filedialog.cpp"
        "include/GraphScriptEditor.h"
        "include/imgui_filedialog.h"
        "include/Utils.h"
        )

# glad
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "${CMAKE_DL_LIBS}")

# glfw
target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# imgui
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_DIR}/include")
target_link_libraries(${PROJECT_NAME} "imgui" "${CMAKE_DL_LIBS}")

# graphscript
target_link_libraries(${PROJECT_NAME} GraphScript)
target_include_directories(${PROJECT_NAME} PRIVATE "${ROOT_DIR}/graphscript/include" "include")

