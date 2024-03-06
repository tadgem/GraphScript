cmake_minimum_required(VERSION 3.17)
project(GraphScript-Runtime)

set(CMAKE_CXX_STANDARD 17)
add_executable(GraphScript-Runtime "src/main.cpp" "New.gs" "New___Entry.gse")

# graphscript-raylib
target_link_libraries(${PROJECT_NAME} GraphScript GraphScript-Raylib GraphScript-Runtime-Shared)
target_include_directories(${PROJECT_NAME} PRIVATE "${ROOT_DIR}/graphscript/include" "include" ${GRAPHSCRIPT_RAYLIB_HEADERS} ${GRAPHSCRIPT_RUNTIME_SHARED_HEADERS})

get_filename_component(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/New.gs
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/New___Entry.gse
DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_custom_command(TARGET GraphScript-Runtime 
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:GraphScript-Runtime> "${ROOT_DIR}/sandbox/sample")