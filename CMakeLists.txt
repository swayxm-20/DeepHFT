cmake_minimum_required(VERSION 3.10)
project(DeepHFT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -Wall -pthread")

include_directories(include)

add_executable(deep_hft
    src/main.cpp
    src/order_book.cpp
)