cmake_minimum_required(VERSION 3.5.0)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Projects
project(dataSplitter CXX)
project(dataSplitter-IGCStream CXX)
project(FBFX-Test CXX)
project(RETH CXX)
project(STR-HASH-test CXX)
project(RenderEclipseTools CXX)

# Flags
set(CMAKE_CXX_FLAGS "-Wall -O2")
set(CMAKE_CXX_FLAGS_RELEASE "-g0")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-s")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Paths
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

option(BUILD_REE "Build RenderEclipse Tools" OFF)

if (BUILD_REE)
    find_package(ZLIB)
    find_package(wxWidgets COMPONENTS core base CONFIG)

    # Include directories and link libraries
    include_directories(${wxWidgets_INCLUDE_DIRS})
    add_definitions(${wxWidgets_CXX_FLAGS})

    add_executable(RenderEclipseTools
        REE/main.cpp
        REE/ARC.cpp
        REE/reth.cpp
        REE/fileLoader.cpp
        REE/wxwindow.cpp
        REE/REE.rc
    )

    target_link_libraries(RenderEclipseTools PRIVATE ${wxWidgets_LIBRARIES} ZLIB::ZLIB )
endif()
