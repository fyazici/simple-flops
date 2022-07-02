project('flops', 'cpp')

cmake = import('cmake')
fmt_dep = cmake.subproject('fmt').dependency('fmt')
argparse_dep = cmake.subproject('argparse').dependency('argparse')

srcs = [
    'flops.cpp'
]

deps = [
    dependency('threads'),
    fmt_dep,
    argparse_dep
]

executable('flops', srcs, dependencies: deps, override_options: ['cpp_std=c++17'])