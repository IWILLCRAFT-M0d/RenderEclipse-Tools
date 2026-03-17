cmake_minimum_required(VERSION 3.5.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Projects
project(dataSplitter LANGUAGES CXX VERSION 0.3)
project(dataSplitter-IGCStream LANGUAGES CXX VERSION 0.1)
project(FBFX-Test LANGUAGES CXX VERSION 0.2)
project(RETH LANGUAGES CXX VERSION 0.2)
project(STR-HASH-test LANGUAGES CXX VERSION 0.1)
project(CAM LANGUAGES CXX VERSION 2.1)

# Flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -O2 -static-libgcc -static-libstdc++ -static")
set(CMAKE_CXX_FLAGS_RELEASE "-g0")
set(CMAKE_CXX_FLAGS_DEBUG "-g2 -DDEBUG -DDEBOOG")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")

set(BUILD_SHARED_LIBS OFF)
set(BUILD_STATIC_LIBS ON)

# Setup extra tools build
message("Setting extra tools build")
add_executable(dataSplitter
    dataSplitter/dataSplitter.cpp
)

add_executable(dataSplitter-IGCStream
    dataSplitter/dataSplitter_IGCStream.cpp
)

add_executable(FBXF-test
    FBXFtest/FBXFtest.cpp
)

add_executable(RETH
    RETH/SourceRETH.cpp
)

add_executable(Str-Hash-Test
    "String Hash Test/StrHashTest.cpp"
)

# Setup Climax ARC Manager (CAM) build
message("Setting Climax ARC Manager (CAM) build")
include(FetchContent)
set(wxBUILD_SHARED OFF)
set(ZLIB_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ZLIB_BUILD_SHARED OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    wxWidgets
    GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
    GIT_SHALLOW ON
    GIT_TAG "v3.3.2"
)
FetchContent_Declare(
    zlib-cmake
    URL https://github.com/jimmy-park/zlib-cmake/archive/main.tar.gz
)
FetchContent_MakeAvailable(wxWidgets)
FetchContent_MakeAvailable(zlib-cmake)

add_executable(CAM
    CAM/main.cpp
    CAM/ARC.cpp
    CAM/fileLoader.cpp
    CAM/wxwindow.cpp
    CAM/Icon.rc
)

target_link_libraries(CAM
    PRIVATE ZLIB::ZLIB
    PRIVATE wx::core
    PRIVATE wx::net
    PRIVATE wx::base
)
