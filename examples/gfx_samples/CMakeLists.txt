cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS ${CMAKE_CURRENT_LIST_DIR}/../../sdk)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
# Determines the final filename like /build/gfx_samples.app.elf
project(gfx_samples)

include(elf_loader)
project_elf(gfx_samples_elf)

target_link_options(${CMAKE_PROJECT_NAME}.elf PRIVATE -Wl,--unresolved-symbols=ignore-all)