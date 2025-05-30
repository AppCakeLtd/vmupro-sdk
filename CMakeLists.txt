cmake_minimum_required(VERSION 3.13)

project(vmu_sdk_example C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdlib -ffreestanding -fPIC")

# Stub library
add_library(vmu_stub STATIC sdk/vmu_stub.c)
target_include_directories(vmu_stub PUBLIC ${CMAKE_SOURCE_DIR}/sdk/include)

# Add all examples
add_subdirectory(examples/minimal)