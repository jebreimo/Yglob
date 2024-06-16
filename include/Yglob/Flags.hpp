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
        CASE_SENSITIVE = 1,
        NO_BRACES = 2,
        NO_SETS = 4
    };

    YGLOB_ENABLE_BITMASK_OPERATORS(GlobFlags);

    enum class PathIteratorFlags : unsigned
    {
        DEFAULT = 0,
        CASE_SENSITIVE = 1,
        NO_BRACES = 2,
        NO_SETS = 4,
        NO_FILES = 8,
        NO_DIRECTORIES = 16
    };

    YGLOB_ENABLE_BITMASK_OPERATORS(PathIteratorFlags);
}
