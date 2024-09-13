cmake_minimum_required(VERSION 3.5.0)
project(RenderEclipseTools VERSION 1.0 LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)


add_subdirectory(wxWidgets)
add_subdirectory(zlib)

add_executable(RenderEclipseTools WIN32
    src/main.cpp
    src/ARC.cpp
    src/reth.cpp
    src/wxwindow.cpp
    src/REE.rc
)

target_link_libraries(RenderEclipseTools 
    wx::net
    wx::core
    wx::base
    zlib
)