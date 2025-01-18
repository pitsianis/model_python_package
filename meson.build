project('mylib', 'c')

# Define dependencies
fftw_dep = dependency('fftw3', required: true)

# Build library
library('mylib',
  'mylib.c',
  dependencies: fftw_dep,
  install: true
)
