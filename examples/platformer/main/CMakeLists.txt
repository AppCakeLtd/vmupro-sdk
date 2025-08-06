idf_component_register(SRCS "platformer.c" "framerate.c" "mixer.c"
                       INCLUDE_DIRS ".")

# Prevent linking full IDF runtime
target_compile_options(${COMPONENT_LIB} PRIVATE -nostartfiles -nostdlib -fno-builtin)
