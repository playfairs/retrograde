project('retrograde', 'cpp',
  version : '0.1.0',
  default_options : [ 'warning_level=3', 'buildtype=debugoptimized', 'cpp_std=c++23' ]
)

qt6_dep = dependency('qt6', modules : ['Core', 'Widgets'])
qt6 = import('qt6')

moc_files = qt6.preprocess(
  moc_headers : ['src/gui/retrograde.h'],
  moc_sources : ['src/gui/main.cpp']
)

executable('retrograde-gui',
  'src/gui/main.cpp',
  'src/gui/retrograde.cpp',
  'src/wrapper/wrapper.cpp',
  moc_files,
  dependencies : qt6_dep,
  install : true)

executable('retrograde',
  'src/main.cpp',
  install : true)
