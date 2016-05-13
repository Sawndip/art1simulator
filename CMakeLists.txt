cmake_minimum_required(VERSION 2.8)

project(art1)

FILE(GLOB SRCS src/*.c ccl/list.c ccl/vector.c ccl/error.c ccl/heap.c ccl/observer.c ccl/qsortex.c ccl/memorymanager.c ccl/iMask.c)

add_executable(
    art1
    ${SRCS})

add_definitions(-Wall -oO -g)

include_directories(ccl)

link_directories(${CMAKE_BINARY_DIR}/res)

target_link_libraries(art1 m)
