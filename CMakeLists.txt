cmake_minimum_required(VERSION 3.13)

project(vmu_sdk_example C)
set(CMAKE_C_STANDARD 11)

# Set top-level flags for freestanding build
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
  -nostdlib \
  -ffreestanding \
  -fPIC \
  -fno-common \
  -fno-jump-tables \
  -mlongcalls \
  -mtext-section-literals \
  -fdata-sections \
  -ffunction-sections \
  -fvisibility=hidden \
  -Wall -Wextra")

# Stub library used by all apps
add_library(vmu_stub STATIC sdk/vmu_stub.c)
target_include_directories(vmu_stub PUBLIC ${CMAKE_SOURCE_DIR}/sdk/include)

# Add example apps
add_subdirectory(examples/minimal)
