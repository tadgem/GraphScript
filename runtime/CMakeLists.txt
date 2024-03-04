cmake_minimum_required(VERSION 3.17)
project(GraphScript-Runtime)

set(CMAKE_CXX_STANDARD 17)
add_executable(GraphScript-Runtime "src/main.cpp")

# graphscript-raylib
target_link_libraries(${PROJECT_NAME} GraphScript GraphScript-Raylib GraphScript-Runtime-Shared)
target_include_directories(${PROJECT_NAME} PRIVATE "${ROOT_DIR}/graphscript/include" "include" ${GRAPHSCRIPT_RAYLIB_HEADERS} ${GRAPHSCRIPT_RUNTIME_SHARED_HEADERS})

