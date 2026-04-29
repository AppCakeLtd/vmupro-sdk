cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/../../sdk/c)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# Determines the final filename like /build/vmupro_network.app.elf
project(network_example)

include(elf_loader)
project_elf(network_example)

target_link_options(${CMAKE_PROJECT_NAME}.elf PRIVATE -Wl,--unresolved-symbols=ignore-all)
