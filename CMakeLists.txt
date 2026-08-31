cmake_minimum_required(VERSION 3.10)
project(embedded_c_ring_buffer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include_directories(include)

add_library(ring_buffer STATIC src/ring_buffer.c)

add_executable(test_runner tests/test_main.c)
target_link_libraries(test_runner PRIVATE ring_buffer)
