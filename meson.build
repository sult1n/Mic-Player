project('mic-player', 'cpp', version: '0.1', default_options : ['warning_level=3', 'cpp_std=c++20', 'werror=false'])

extra_args = [
  '-Wpedantic',
  '-Wshadow',
  '-Wnon-virtual-dtor',
  '-Wold-style-cast',
  '-Wcast-align',
  '-Wunused',
  '-Woverloaded-virtual',
  '-Wconversion',
  '-Wsign-conversion',
  '-Wnull-dereference',
  '-Wdouble-promotion',
  '-Wformat=2',
]

src = files('src/main.cpp',
            'src/audio.cpp',
            'src/tui.cpp')

miniaudio_inc = include_directories('miniaudio', is_system: true)
notcurses_dep = dependency('notcurses')
notcursespp_dep = dependency('notcurses++')

executable('mic-player', src, cpp_args: extra_args, include_directories : [miniaudio_inc], dependencies : [notcurses_dep, notcursespp_dep], install: true)
