# ===========================================================================
# Copyright © 2024 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2024-06-11.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.28)
project(Yglob)

set(CMAKE_CXX_STANDARD 20)

# Set YGLOB_MASTER_PROJECT to ON if Yglob is the root level project.
if (NOT DEFINED YGLOB_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(YGLOB_MASTER_PROJECT ON)
    else ()
        set(YGLOB_MASTER_PROJECT OFF)
    endif ()
endif ()

# Test option
option(YGLOB_BUILD_TESTS "Build tests" ${YGLOB_MASTER_PROJECT})

include(FetchContent)
FetchContent_Declare(ystring
    GIT_REPOSITORY "https://github.com/jebreimo/Ystring2.git"
    GIT_TAG "master"
)
FetchContent_MakeAvailable(ystring)

add_library(Yglob STATIC
    include/Yglob/YglobDefinitions.hpp
    include/Yglob/GlobMatcher.hpp
    src/Yglob/GlobMatcher.cpp
    src/Yglob/GlobPattern.cpp
    src/Yglob/PathMatcher.cpp
    include/Yglob/PathMatcher.hpp
)

target_link_libraries(Yglob
    PRIVATE
        Ystring2::Ystring
)

target_include_directories(Yglob BEFORE
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

if (YGLOB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/YglobTest)
endif ()
