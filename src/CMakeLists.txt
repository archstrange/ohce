aux_source_directory(. SRC)

add_library(ohce ${SRC})
target_link_libraries(ohce sad cutils)
