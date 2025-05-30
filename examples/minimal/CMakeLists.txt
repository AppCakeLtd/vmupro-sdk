set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -fno-jump-tables -mlongcalls -fdata-sections -ffunction-sections")

add_executable(minimal.elf vmupro_main.c)
target_include_directories(minimal.elf PRIVATE ${CMAKE_SOURCE_DIR}/sdk/include)
target_link_libraries(minimal.elf vmu_stub)

# Linker script
target_link_options(minimal.elf PRIVATE "-T${CMAKE_SOURCE_DIR}/sdk/vmupro.ld")

add_custom_command(TARGET minimal.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary minimal.elf minimal.bin
    COMMENT "Converting minimal.elf to minimal.bin"
)