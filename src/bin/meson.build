bin_src = [
  'main.c',
]

ohce_bin = executable(
  'ohce',
  bin_src,
  install: true,
  link_with: ohce_lib,
  dependencies: [ libsad, libcutils ]
)
