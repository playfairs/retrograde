cmake_minimum_required(VERSION 3.16)
project(retrograde-gui)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

add_executable(retrograde-gui
    main.cpp
    ../wrapper/wrapper.cpp
)

target_include_directories(retrograde-gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../wrapper
)

target_link_libraries(retrograde-gui PRIVATE
    Qt6::Core
    Qt6::Widgets
)
