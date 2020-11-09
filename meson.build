project ('ohce', 'C')

cc = meson.get_compiler('c')

libcutils = cc.find_library('cutils')
libsad = cc.find_library('sad')

subdir('src/lib')
subdir('src/bin')

