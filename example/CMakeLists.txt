cmake_minimum_required(VERSION 3.17)
project(GraphScript-Example)

set(CMAKE_CXX_STANDARD 17)
add_executable(GraphScript-Example "src/main.cpp")

target_link_libraries(${PROJECT_NAME} GraphScript)
target_include_directories(${PROJECT_NAME} PRIVATE "${ROOT_DIR}/graphscript/include")

