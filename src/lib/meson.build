lib_src = [
  'eval.c',
  'idkeyword.c',
  'io.c',
  'primitive.c',
]

lib_headers = [
  'eval.h',
  'io.h',
  'option.h',
]

ohce_lib = static_library(
  'ohce',
  lib_src,
  dependencies: [ libsad, libcutils ],
  install: true
)

install_headers(
  lib_headers,
  subdir: 'ohce'
)
