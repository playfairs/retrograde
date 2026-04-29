project('retrograde-gui', 'cpp',
  version : '0.1.0',
  default_options : ['warning_level=3',
                     'cpp_std=c++20'])

qt6_dep = dependency('qt6', modules : ['Core', 'Widgets'])

qt6 = import('qt6')
moc_files = qt6.preprocess(moc_headers : ['retrograde.h'],
                           moc_sources : ['retrograde.cpp'])

executable('retrograde-gui',
  'main.cpp',
  'retrograde.cpp',
  moc_files,
  '../wrapper/wrapper.cpp',
  include_directories : ['..'],
  dependencies : qt6_dep,
  install : true)
