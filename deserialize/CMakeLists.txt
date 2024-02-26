cmake_minimum_required(VERSION 3.17)
project(GraphScript-Deserialize)

set(CMAKE_CXX_STANDARD 17)
add_executable(GraphScript-Deserialize "src/main.cpp" output.gs)

target_link_libraries(${PROJECT_NAME} GraphScript)
target_include_directories(${PROJECT_NAME} PRIVATE "${ROOT_DIR}/graphscript/include")

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/output.gs
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})