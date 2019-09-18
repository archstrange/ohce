aux_source_directory(. SRC)

add_executable(ohce ${SRC})
target_link_libraries(ohce sad cutils)
