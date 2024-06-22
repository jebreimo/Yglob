//****************************************************************************
// Copyright © 2024 Jan Erik Breimo. All rights reserved.
// Created by Jan Erik Breimo on 2024-03-25.
//
// This file is distributed under the BSD License.
// License text is included with the source distribution.
//****************************************************************************
#pragma once

#include "BitmaskOperators.hpp"

namespace Yglob
{
    enum class GlobFlags
    {
        DEFAULT = 0,
        NO_BRACES = 1,
        NO_SETS = 2,
        CASE_SENSITIVE = 4
    };

    YGLOB_ENABLE_BITMASK_OPERATORS(GlobFlags);

    enum class PathIteratorFlags : unsigned
    {
        DEFAULT = 0,
        NO_BRACES = 1,
        NO_SETS = 2,
        /**
         * @brief If set, the glob patterns will be treated as case-sensitive.
         *
         * By default, matching of the glob parts of the path expression
         * (i.e. any directory or file name containing `*`, `?`, `[]` or `{}`,
         * and everything following a `**` glob pattern) is case-insensitive.
         */
        CASE_SENSITIVE_GLOBS = 4,
        /**
         * @brief If set, the path iterator will treat the non-glob parts of
         *      the path as case-insensitive.
         *
         * The default comparison method for the directory and file names in
         * the path expression depends on the operating system and file
         * system. On Windows, the comparison is case-insensitive, on Unix-like
         * systems it is case-sensitive.
         *
         * Enabling this flag can slow down the path iteration, especially
         * in directories with many files and sub-directories.
         */
        CASE_INSENSITIVE_PATHS = 8,
        NO_FILES = 16,
        NO_DIRECTORIES = 32
    };

    YGLOB_ENABLE_BITMASK_OPERATORS(PathIteratorFlags);
}
