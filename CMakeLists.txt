# ===========================================================================
# Copyright © 2024 Jan Erik Breimo. All rights reserved.
# Created by Jan Erik Breimo on 2024-06-11.
#
# This file is distributed under the BSD License.
# License text is included with the source distribution.
# ===========================================================================
cmake_minimum_required(VERSION 3.28)
project(Yglob VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 20)

# Set YGLOB_MASTER_PROJECT to ON if Yglob is the root level project.
if (NOT DEFINED YGLOB_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(YGLOB_MASTER_PROJECT ON)
    else ()
        set(YGLOB_MASTER_PROJECT OFF)
    endif ()
endif ()

# Build tests option
option(YGLOB_BUILD_TESTS "Build tests" ${YGLOB_MASTER_PROJECT})

# Build extras option
option(YGLOB_BUILD_EXTRAS "Build extras" ${YGLOB_MASTER_PROJECT})

# Install option
option(YGLOB_INSTALL "Generate the install target" ${YGLOB_MASTER_PROJECT})

if (YGLOB_INSTALL)
    set(YSTRING_INSTALL ON)
endif ()

include(FetchContent)
FetchContent_Declare(ystring
    GIT_REPOSITORY "https://github.com/jebreimo/Ystring2.git"
    GIT_TAG "v2.3.1"
)
FetchContent_MakeAvailable(ystring)

add_library(Yglob STATIC
    include/Yglob/BitmaskOperators.hpp
    include/Yglob/Flags.hpp
    include/Yglob/GlobMatcher.hpp
    include/Yglob/PathIterator.hpp
    include/Yglob/PathMatcher.hpp
    include/Yglob/Yglob.hpp
    include/Yglob/YglobDefinitions.hpp
    include/Yglob/YglobException.hpp
    src/Yglob/GlobElements.cpp
    src/Yglob/GlobElements.hpp
    src/Yglob/GlobMatcher.cpp
    src/Yglob/MatchGlobPattern.cpp
    src/Yglob/MatchGlobPattern.hpp
    src/Yglob/ParseGlobPattern.cpp
    src/Yglob/ParseGlobPattern.hpp
    src/Yglob/PathIterator.cpp
    src/Yglob/PathMatcher.cpp
    src/Yglob/PathPartIterator.cpp
    src/Yglob/PathPartIterator.hpp
)

target_link_libraries(Yglob
    PRIVATE
        Ystring2::Ystring
)

include(GNUInstallDirs)

target_include_directories(Yglob BEFORE
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

add_library(Yglob::Yglob ALIAS Yglob)

if (YGLOB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/YglobTest)
endif ()

if (YGLOB_BUILD_EXTRAS)
    add_subdirectory(extras/listfiles)
endif ()

export(TARGETS Yglob
    NAMESPACE Yglob::
    FILE YglobConfig.cmake)

if (YGLOB_INSTALL)
    install(TARGETS Yglob
        EXPORT YglobConfig
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    install(EXPORT YglobConfig
        FILE YglobConfig.cmake
        NAMESPACE Yglob::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yglob
    )

    file(GLOB_RECURSE PUBLIC_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
    install(
        FILES
            ${PUBLIC_INCLUDES}
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}/Yglob
    )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(YglobConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
    )

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/YglobConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Yglob
    )
endif ()
