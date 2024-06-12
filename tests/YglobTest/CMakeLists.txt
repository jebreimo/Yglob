# ===========================================================================
# Copyright © 2024 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2024-06-12.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.28)

include(FetchContent)
FetchContent_Declare(catch
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v3.4.0"
    )
FetchContent_MakeAvailable(catch)

add_executable(YglobTest
    Utf8Chars.hpp
    U8Adapter.hpp
    test_GlobMatcher.cpp
    test_GlobPattern.cpp
    test_PathMatcher.cpp
)

target_link_libraries(YglobTest
    PRIVATE
        Yglob::Yglob
        Ystring2::Ystring
        Catch2::Catch2WithMain
    )

target_compile_options(YglobTest
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
    )

target_include_directories(YglobTest
    PRIVATE
        ../../src
    )

ystring_enable_all_warnings(YglobTest)

add_test(NAME YglobTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/YglobTest)
