cmake_minimum_required(VERSION 3.10)
project(WorkCounter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(Threads REQUIRED)

add_executable(word_counter_secuencial secuencial/word_counter_secuencial.cpp)

add_executable(word_counter_paralelo paralelo/word_counter_paralelo.cpp)
target_link_libraries(word_counter_paralelo PRIVATE Threads::Threads)
