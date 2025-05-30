# esp32s3-toolchain.cmake

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR xtensa)

set(CMAKE_C_COMPILER xtensa-esp32s3-elf-gcc)
set(CMAKE_CXX_COMPILER xtensa-esp32s3-elf-g++)
set(CMAKE_ASM_COMPILER xtensa-esp32s3-elf-gcc)

set(CMAKE_OBJCOPY xtensa-esp32s3-elf-objcopy)
