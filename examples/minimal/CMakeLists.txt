cmake_minimum_required(VERSION 3.13)
project(vmu_sdk_minimal_example C)

set(CMAKE_C_STANDARD 11)

# Flags to avoid linker relaxation and unsafe instruction usage
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} \
  -fPIC \
  -fno-common \
  -fno-jump-tables \
  -mlongcalls \
  -mtext-section-literals \
  -fdata-sections \
  -ffunction-sections \
  -fvisibility=hidden \
  -Wall -Wextra \
  -Wl,--no-relax")

add_executable(minimal.elf vmupro_main.c)

target_include_directories(minimal.elf PRIVATE ${CMAKE_SOURCE_DIR}/sdk/include)
target_link_libraries(minimal.elf vmu_stub)

# Use custom linker script
target_link_options(minimal.elf PRIVATE "-T${CMAKE_SOURCE_DIR}/sdk/vmupro.ld")

# Convert ELF to BIN
add_custom_command(TARGET minimal.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary minimal.elf minimal.bin
    COMMENT "Converting minimal.elf to minimal.bin"
)
