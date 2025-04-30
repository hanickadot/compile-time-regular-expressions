project(
  'ctre',
  'cpp',
  version: '3.9.0',
  license: 'Apache-2.0',
  default_options: ['cpp_std=gnu++20,gnu++17,vc++latest,c++20,c++17'],
)

subdir('include')

ctre_dep = declare_dependency(
  include_directories: inc,
  sources: hdrs,
)

meson.override_dependency('ctre', ctre_dep)

pkgconf = import('pkgconfig')
pkgconf.generate(
  name: 'ctre',
  description: 'Fast compile-time regular expressions with support for matching/searching/capturing during compile-time or runtime.',
)
