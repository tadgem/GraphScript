cmake_minimum_required(VERSION 3.10)

set(PROJECT_N GraphScript)
project(${PROJECT_N} VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE INCLUDES ${PROJECT_SOURCE_DIR}/include/*.h)

foreach(file ${SRCS})
        message(STATUS ${file})
endforeach()

foreach(dir ${INCLUDES})
        message(STATUS ${dir})
endforeach()

add_library(${PROJECT_N} STATIC ${INCLUDES} ${SRCS})

target_include_directories(${PROJECT_N} PUBLIC include)
